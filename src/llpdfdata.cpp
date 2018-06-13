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
 * \file llpdfdata.cpp
 * \class PdfData
 *
 * A class to handle PDF data.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PDFDATA

/** Define a function's name (_fun) with prefix PdfData */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a PdfData*.
 *
 * \param L Lua state
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    PdfData *pd = ll_take_udata<PdfData>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "pd", reinterpret_cast<void *>(pd));
    /*
     * PDF data is destructed internally by
     * static void pdfdataDestroy(L_PDF_DATA  **plpd)
     */
    return 0;
}

/**
 * \brief Create a new PdfData*.
 * \param L Lua state
 * \return 1 PdfData* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    PdfData *pd = ll_calloc<PdfData>(_fun, L, 1);
    return ll_push_PdfData(_fun, L, pd);
}

/**
 * \brief Printable string for a PdfData* (%pdd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L Lua state
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    PdfData *pdd = ll_check_PdfData(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!pdd) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p", reinterpret_cast<void *>(pdd));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF,
                 "\n    title             : %s", pdd->title ? pdd->title : "<none>");
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    n                 : %d", pdd->n);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    ncmap             : %d", pdd->ncmap);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    cida              : %p", reinterpret_cast<void *>(pdd->cida));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    id                : %s", pdd->id ? pdd->id : "<none>");
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    catalog string    : %s", pdd->obj1 ? pdd->obj1 : "<none>");
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    metadata string   : %s", pdd->obj2 ? pdd->obj2 : "<none>");
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    pages string      : %s", pdd->obj3 ? pdd->obj3 : "<none>");
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    page string       : %s", pdd->obj4 ? pdd->obj4 : "<none>");
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    content string    : %s", pdd->obj5 ? pdd->obj5 : "<none>");
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    poststream        : %p", reinterpret_cast<void *>(pdd->poststream));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    trailer           : %p", reinterpret_cast<void *>(pdd->trailer));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    xy                : " LL_PTA "* %p", reinterpret_cast<void *>(pdd->xy));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    wh                : " LL_PTA "* %p", reinterpret_cast<void *>(pdd->wh));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    mediabox          : " LL_BOX "* %p", reinterpret_cast<void *>(pdd->mediabox));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    saprex            : " LL_SARRAY "* %p", reinterpret_cast<void *>(pdd->saprex));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    sacmap            : " LL_SARRAY "* %p", reinterpret_cast<void *>(pdd->sacmap));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    objsize           : " LL_DNA "* %p", reinterpret_cast<void *>(pdd->objsize));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    objloc            : " LL_DNA "* %p", reinterpret_cast<void *>(pdd->objloc));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    xrefloc           : %d", pdd->xrefloc);
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a string (filein).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a string (fileout).
 * Arg #5 is expected to be a l_int32 (x).
 * Arg #6 is expected to be a l_int32 (y).
 * Arg #7 is expected to be a l_int32 (res).
 * Arg #8 is expected to be a string (title).
 * Arg #9 is expected to be a l_int32 (position).
 * </pre>
 * \param L Lua state
 * \return 1 on the Lua stack
 */
static int
ConvertToPdf(lua_State *L)
{
    LL_FUNC("ConvertToPdf");
    const char *filein = ll_check_string(_fun, L, 1);
    l_int32 type = ll_check_encoding(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    const char *fileout = ll_check_string(_fun, L, 4);
    l_int32 x = ll_check_l_int32(_fun, L, 5);
    l_int32 y = ll_check_l_int32(_fun, L, 6);
    l_int32 res = ll_check_l_int32(_fun, L, 7);
    const char *title = ll_check_string(_fun, L, 8);
    PdfData *lpd = nullptr;
    l_int32 position = ll_check_position(_fun, L, 9);
    if (convertToPdf(filein, type, quality, fileout, x, y, res, title, &lpd, position))
        return ll_push_nil(L);
    ll_push_PdfData(_fun, L, lpd);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a string (filein).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (x).
 * Arg #5 is expected to be a l_int32 (y).
 * Arg #6 is expected to be a l_int32 (res).
 * Arg #7 is expected to be a string (title).
 * Arg #8 is expected to be a l_int32 (position).
 * </pre>
 * \param L Lua state
 * \return 2 lstring (%data, %nbytes) and PdfData* (%lpd) on the Lua stack
 */
static int
ConvertToPdfData(lua_State *L)
{
    LL_FUNC("ConvertToPdfData");
    const char *filein = ll_check_string(_fun, L, 1);
    l_int32 type = ll_check_encoding(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 x = ll_check_l_int32(_fun, L, 4);
    l_int32 y = ll_check_l_int32(_fun, L, 5);
    l_int32 res = ll_check_l_int32(_fun, L, 6);
    const char *title = ll_check_string(_fun, L, 7);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    PdfData *lpd = nullptr;
    l_int32 position = ll_check_l_int32(_fun, L, 8);
    if (convertToPdfData(filein, type, quality, &data, &nbytes, x, y, res, title, &lpd, position))
        return ll_push_nil(L);
    ll_push_bytes(_fun, L, data, nbytes);
    ll_push_PdfData(_fun, L, lpd);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a string (filein).
 * Arg #2 is expected to be a l_int32 (res).
 * Arg #3 is expected to be a l_int32 (type).
 * Arg #4 is expected to be a l_int32 (thresh).
 * Arg #5 is expected to be a Boxa* (boxa).
 * Arg #6 is expected to be a l_int32 (quality).
 * Arg #7 is expected to be a l_float32 (scalefactor).
 * Arg #8 is expected to be a string (title).
 * </pre>
 * \param L Lua state
 * \return 1 lstring (%data, %nbytes) on the Lua stack
 */
static int
ConvertToPdfDataSegmented(lua_State *L)
{
    LL_FUNC("ConvertToPdfDataSegmented");
    const char *filein = ll_check_string(_fun, L, 1);
    l_int32 res = ll_check_l_int32(_fun, L, 2);
    l_int32 type = ll_check_encoding(_fun, L, 3);
    l_int32 thresh = ll_check_l_int32(_fun, L, 4);
    Boxa *boxa = ll_check_Boxa(_fun, L, 5);
    l_int32 quality = ll_check_l_int32(_fun, L, 6);
    l_float32 scalefactor = ll_check_l_float32(_fun, L, 7);
    const char *title = ll_check_string(_fun, L, 8);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    if (convertToPdfDataSegmented(filein, res, type, thresh, boxa, quality, scalefactor, title, &data, &nbytes))
        return ll_push_nil(L);
    ll_push_bytes(_fun, L, data, nbytes);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a string (dirname).
 * Arg #2 is expected to be a string (substr).
 * Arg #3 is expected to be a string (title).
 * Arg #4 is expected to be a string (fileout).
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
ConvertUnscaledFilesToPdf(lua_State *L)
{
    LL_FUNC("ConvertUnscaledFilesToPdf");
    const char *dirname = ll_check_string(_fun, L, 1);
    const char *substr = ll_check_string(_fun, L, 2);
    const char *title = ll_check_string(_fun, L, 3);
    const char *fileout = ll_check_string(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == convertUnscaledFilesToPdf(dirname, substr, title, fileout));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (fname).
 * Arg #2 is expected to be a string (title).
 * </pre>
 * \param L Lua state
 * \return 1 lstring (%data, %nbytes) on the Lua stack
 */
static int
ConvertUnscaledToPdfData(lua_State *L)
{
    LL_FUNC("ConvertUnscaledToPdfData");
    const char *fname = ll_check_string(_fun, L, 1);
    const char *title = ll_check_string(_fun, L, 2);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    if (convertUnscaledToPdfData(fname, title, &data, &nbytes))
        return ll_push_nil(L);
    ll_push_bytes(_fun, L, data, nbytes);
    ll_free(data);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class PdfData.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PdfData* contained in the user data
 */
PdfData *
ll_check_PdfData(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PdfData>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PdfData* contained in the user data
 */
PdfData *
ll_opt_PdfData(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_PdfData(_fun, L, arg);
}

/**
 * \brief Push BMF user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state
 * \param cd pointer to the L_PdfData
 * \return 1 PdfData* on the Lua stack
 */
int
ll_push_PdfData(const char *_fun, lua_State *L, PdfData *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, cd);
}
/**
 * \brief Create and push a new PdfData*.
 * \param L Lua state
 * \return 1 PdfData* on the Lua stack
 */
int
ll_new_PdfData(lua_State *L)
{
    FUNC("ll_new_PdfData");
    PdfData *pd = ll_calloc<PdfData>(_fun, L, 1);
    return ll_push_PdfData(_fun, L, pd);
}

/**
 * \brief Register the PdfData methods and functions in the PdfData meta table.
 * \param L Lua state
 * \return 1 table on the Lua stack
 */
int
ll_open_PdfData(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                        Destroy},
        {"__new",                       ll_new_PdfData},
        {"__tostring",                  toString},
        {"Create",                      Create},
        {"Destroy",                     Destroy},
        {"ConvertToPdf",                ConvertToPdf},
        {"ConvertToPdfData",            ConvertToPdfData},
        {"ConvertToPdfDataSegmented",   ConvertToPdfDataSegmented},
        {"ConvertUnscaledFilesToPdf",   ConvertUnscaledFilesToPdf},
        {"ConvertUnscaledToPdfData",    ConvertUnscaledToPdfData},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_PdfData);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
