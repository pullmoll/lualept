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
 * \file llcompdata.cpp
 * \class CompData
 *
 * A class to handle compressed data.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_COMPDATA

/** Define a function's name (_fun) with prefix CompData */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a CompData*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CompData* (cid).
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    CompData *cid = ll_take_udata<CompData>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "cid", reinterpret_cast<void *>(cid));
    l_CIDataDestroy(&cid);
    return 0;
}

/**
 * \brief Printable string for a CompData* (%cid).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    CompData *cid = ll_check_CompData(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!cid) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p", reinterpret_cast<void *>(cid));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s",
                 "type", ll_string_encoding(cid->type));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %p",
                 "datacomp", reinterpret_cast<void *>(cid->datacomp));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %" PRIu64,
                 "nbytescomp", static_cast<l_uintptr_t>(cid->nbytescomp));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %p",
                 "data85", reinterpret_cast<void *>(cid->data85));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %" PRIu64,
                 "nbytes85", static_cast<l_uintptr_t>(cid->nbytes85));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %p",
                 "cmapdata85", reinterpret_cast<void *>(cid->cmapdata85));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %p",
                 "cmapdatahex", reinterpret_cast<void *>(cid->cmapdatahex));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "ncolors", cid->ncolors);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "w", cid->w);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "h", cid->h);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "bps", cid->bps);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "spp", cid->spp);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "minisblack", cid->minisblack);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "predictor", cid->predictor);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %" PRIu64,
                 "nbytes", static_cast<l_uintptr_t>(cid->nbytes));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "res", cid->res);
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Convert CompData* (%cid) to PDF data in a lstring (%data, %nbytes).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CompData* (cid).
 * Arg #2 is expected to be a string (title).
 *
 * Leptonica's Notes:
 *      (1) Caller must not destroy the cid.  It is absorbed in the
 *          lpd and destroyed by this function.
 * </pre>
 * \param L Lua state.
 * \return 1 lstring on the Lua stack.
 */
static int
ConvertToPdfData(lua_State *L)
{
    LL_FUNC("ConvertToPdfData");
    CompData *cid = ll_check_CompData(_fun, L, 1);
    const char *title = ll_check_string(_fun, L, 2);
    l_uint8 *data = nullptr;
    size_t nbytes = 0;
    if (cidConvertToPdfData(cid, title, &data, &nbytes))
        return ll_push_nil(_fun, L);
    ll_push_bytes(_fun, L, data, nbytes);
    return 1;
}

/**
 * \brief Create a new CompData* from a file (%fname).
 * <pre>
 * Arg #1 is expected to be a string (fname).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (ascii85).
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    const char *fname = ll_check_string(_fun, L, 1);
    l_int32 type = ll_check_compression(_fun, L, 2, IFF_DEFAULT);
    l_int32 quality = ll_opt_l_int32(_fun, L, 3, 75);
    l_int32 ascii85 = ll_opt_boolean(_fun, L, 4, FALSE);
    CompData *cid = nullptr;
    if (l_generateCIData(fname, type, quality, ascii85, &cid))
        return ll_push_nil(_fun, L);
    ll_push_CompData(_fun, L, cid);
    return 1;
}

/**
 * \brief Create CompData* (%cid) for PDF from a file (%fname).
 * <pre>
 * Arg #1 is expected to be a string (fname).
 * Arg #2 is expected to be a Pix* (pix).
 * Arg #3 is expected to be a l_int32 (quality).
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
CreateForPdf(lua_State *L)
{
    LL_FUNC("CreateForPdf");
    const char *fname = ll_check_string(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    CompData *cid = nullptr;
    if (l_generateCIDataForPdf(fname, pix, quality, &cid))
        return ll_push_nil(_fun, L);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Flate compress data from a file (%fname) to a CompData* (%cid).
 * <pre>
 * Arg #1 is expected to be a string (fname).
 * Arg #2 is expected to be a l_int32 (ascii85flag).
 *
 * Leptonica's Notes:
 *      (1) The input image is converted to one of these 4 types:
 *           ~ 1 bpp
 *           ~ 8 bpp, no colormap
 *           ~ 8 bpp, colormap
 *           ~ 32 bpp rgb
 *      (2) Set ascii85flag:
 *           ~ 0 for binary data (not permitted in PostScript)
 *           ~ 1 for ascii85 (5 for 4) encoded binary data
 * </pre>
 * \param L Lua state.
 * \return 1 CompData* on the Lua stack.
 */
static int
FlateData(lua_State *L)
{
    LL_FUNC("FlateData");
    const char *fname = ll_check_string(_fun, L, 1);
    l_int32 ascii85flag = ll_check_l_int32(_fun, L, 2);
    CompData *cid = l_generateFlateData(fname, ascii85flag);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Flate compress data from a file (%fname) for a PDF to a CompData* (%cid).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (fname).
 * Arg #2 is expected to be a Pix* (pixs).
 *
 * Leptonica's Notes:
 *      (1) If you hand this a png file, you are going to get
 *          png predictors embedded in the flate data. So it has
 *          come to this. http://xkcd.com/1022/
 *      (2) Exception: if the png is interlaced or if it is RGBA,
 *          it will be transcoded.
 *      (3) If transcoding is required, this will not have to read from
 *          file if you also input a pix.
 * </pre>
 * \param L Lua state.
 * \return 1 CompData* on the Lua stack.
 */
static int
FlateDataPdf(lua_State *L)
{
    LL_FUNC("FlateDataPdf");
    const char *fname = ll_check_string(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    CompData *cid = l_generateFlateDataPdf(fname, pixs);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief G4 compress data from a file (%fname) to a CompData* (%cid).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (fname).
 * Arg #2 is expected to be a l_int32 (ascii85flag).
 *
 * Leptonica's Notes:
 *      (1) Set ascii85flag:
 *           ~ 0 for binary data (not permitted in PostScript)
 *           ~ 1 for ascii85 (5 for 4) encoded binary data
 *             (not permitted in pdf)
 * </pre>
 * \param L Lua state.
 * \return 1 CompData* on the Lua stack.
 */
static int
G4Data(lua_State *L)
{
    LL_FUNC("G4Data");
    const char *fname = ll_check_string(_fun, L, 1);
    l_int32 ascii85flag = ll_check_l_int32(_fun, L, 2);
    CompData *cid = l_generateG4Data(fname, ascii85flag);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Generate CompData* (%cid) from a file (%fname).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (ascii85).
 * </pre>
 * \param L Lua state.
 * \return 1 CompData* on the Lua stack.
 */
static int
Generate(lua_State *L)
{
    LL_FUNC("Generate");
    const char *fname = ll_check_string(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 ascii85 = ll_check_l_int32(_fun, L, 4);
    CompData *cid = nullptr;
    if (l_generateCIData(fname, type, quality, ascii85, &cid))
        return ll_push_nil(_fun, L);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Generate CompData* (%cid) from a Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (ascii85).
 *
 * Leptonica's Notes:
 *      (1) Set ascii85:
 *           ~ 0 for binary data (not permitted in PostScript)
 *           ~ 1 for ascii85 (5 for 4) encoded binary data
 * </pre>
 * \param L Lua state.
 * \return 1 CompData* on the Lua stack.
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
        return ll_push_nil(_fun, L);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief JPEG compress data from a file (%fname) to a CompData* (%cid).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (fname).
 * Arg #2 is expected to be a l_int32 (ascii85flag).
 *
 * Leptonica's Notes:
 *      (1) Set ascii85flag:
 *           ~ 0 for binary data (not permitted in PostScript)
 *           ~ 1 for ascii85 (5 for 4) encoded binary data
 *               (not permitted in pdf)
 *      (2) Do not free the data.  l_generateJpegDataMem() will free
 *          the data if it does not use ascii encoding.
 * </pre>
 * \param L Lua state.
 * \return 1 CompData* on the Lua stack.
 */
static int
JpegData(lua_State *L)
{
    LL_FUNC("JpegData");
    const char *fname = ll_check_string(_fun, L, 1);
    l_int32 ascii85flag = ll_check_l_int32(_fun, L, 2);
    CompData *cid = l_generateJpegData(fname, ascii85flag);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief JPEG compress data from a lstring (%str, %nbytes) to a CompData* (%cid).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (str).
 * Arg #2 is expected to be a boolean (ascii85flag).
 *
 * Leptonica's Notes:
 *      (1) See l_generateJpegData().
 * </pre>
 * \param L Lua state.
 * \return 1 CompData* on the Lua stack.
 */
static int
JpegDataMem(lua_State *L)
{
    LL_FUNC("JpegDataMem");
    size_t nbytes = 0;
    const char *str = ll_check_lstring(_fun, L, 1, &nbytes);
    l_int32 ascii85flag = ll_check_boolean(_fun, L, 2);
    l_uint8* data = ll_malloc<l_uint8>(_fun, L, nbytes);
    CompData *cid = nullptr;
    /* copy to data */
    memcpy(data, str, nbytes);
    cid = l_generateJpegDataMem(data, nbytes, ascii85flag);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Check Lua stack at index %arg for user data of class CompData.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CompData* contained in the user data.
 */
CompData *
ll_check_CompData(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<CompData>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CompData* contained in the user data.
 */
CompData *
ll_opt_CompData(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_CompData(_fun, L, arg);
}

/**
 * \brief Push BMF user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param cdata pointer to the L_CompData
 * \return 1 CompData* on the Lua stack.
 */
int
ll_push_CompData(const char *_fun, lua_State *L, CompData *cdata)
{
    if (!cdata)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, cdata);
}

/**
 * \brief Generate and push a new CompData*.
 * \param L Lua state.
 * \return 1 CompData* on the Lua stack.
 */
int
ll_new_CompData(lua_State *L)
{
    FUNC("ll_new_CompData");
    const char *fname = ll_check_string(_fun, L, 1);
    l_int32 type = ll_check_compression(_fun, L, 2, IFF_DEFAULT);
    l_int32 quality = ll_opt_l_int32(_fun, L, 3, 75);
    l_int32 ascii85 = ll_opt_boolean(_fun, L, 4, FALSE);
    CompData *cid = nullptr;

    DBG(LOG_NEW_PARAM, "%s: create for %s = '%s', %s = %s, %s = %d, %s = %s\n", _fun,
        "fname", fname,
        "type", ll_string_compression(type),
        "quality", quality,
        "ascii85", ascii85 ? "true" : "false");
    if (l_generateCIData(fname, type, quality, ascii85, &cid)) {
        DBG(LOG_NEW_PARAM, "%s: failed to create %s*\n", _fun,
            TNAME);
        return ll_push_nil(_fun, L);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(cid));
    ll_push_CompData(_fun, L, cid);
    return 1;
}

/**
 * \brief Register the CompData methods and functions in the CompData meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_CompData(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_CompData},
        {"__tostring",          toString},
        {"ConvertToPdfData",    ConvertToPdfData},
        {"Create",              Create},
        {"CreateForPdf",        CreateForPdf},
        {"Destroy",             Destroy},
        {"FlateData",           FlateData},
        {"FlateDataPdf",        FlateDataPdf},
        {"G4Data",              G4Data},
        {"Generate",            Generate},
        {"GenerateCIData",      GenerateCIData},
        {"JpegData",            JpegData},
        {"JpegDataMem",         JpegDataMem},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_CompData);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
