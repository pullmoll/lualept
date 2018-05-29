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
 *  Lua class CompData
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_COMPDATA */
#define LL_FUNC(x) FUNC(LL_COMPDATA "." x)

/**
 * \brief Destroy a CompData*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CompData* (cid).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    CompData **pcid = ll_check_udata<CompData>(_fun, L, 1, LL_COMPDATA);
    CompData *cid = *pcid;
    DBG(LOG_DESTROY, "%s: '%s' pcid=%p cid=%p\n",
        _fun, LL_COMPDATA, pcid, cid);
    l_CIDataDestroy(&cid);
    *pcid = nullptr;
    return 0;
}

/**
 * \brief Create a new CompData* from a file (%fname).
 * <pre>
 * Arg #1 is expected to be a string (fname).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (ascii85).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    const char *fname = ll_check_string(_fun, L, 1);
    l_int32 type = ll_check_l_int32(_fun, L, 2);
    l_int32 quality = ll_check_l_int32(_fun, L, 3);
    l_int32 ascii85 = ll_check_l_int32(_fun, L, 4);
    CompData *cid = nullptr;
    if (l_generateCIData(fname, type, quality, ascii85, &cid))
        return ll_push_nil(L);
    ll_push_CompData(_fun, L, cid);
    return 1;
}

/**
 * \brief Printable string for a CompData* (%cid).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    CompData *cid = ll_check_CompData(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!cid) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), LL_COMPDATA ": %p\n", reinterpret_cast<void *>(cid));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    type          : %s\n", ll_string_encoding(cid->type));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    datacomp      : %p\n", reinterpret_cast<void *>(cid->datacomp));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    nbytescomp    : %" PRIu64 "\n", static_cast<l_uintptr_t>(cid->nbytescomp));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    data85        : %p\n", reinterpret_cast<void *>(cid->data85));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    nbytes85      : %" PRIu64 "\n", static_cast<l_uintptr_t>(cid->nbytes85));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    cmapdata85    : %p\n", reinterpret_cast<void *>(cid->cmapdata85));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    cmapdatahex   : %p\n", reinterpret_cast<void *>(cid->cmapdatahex));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    ncolors       : %d\n", cid->ncolors);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    w             : %d\n", cid->w);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    h             : %d\n", cid->h);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    bps           : %d\n", cid->bps);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    spp           : %d\n", cid->spp);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    minisblack    : %d\n", cid->minisblack);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    predictor     : %d\n", cid->predictor);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    nbytes        : %" PRIu64 "\n", static_cast<l_uintptr_t>(cid->nbytes));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    res           : %d\n", cid->res);
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Convert CompData* (%cid) to PDF data in a lstring (%data, %nbytes).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CompData* (cid).
 * Arg #2 is expected to be a string (title).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring on the Lua stack
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
        return ll_push_nil(L);
    ll_push_lstring(_fun, L, reinterpret_cast<const char *>(data), nbytes);
    ll_free(data);
    return 1;
}

/**
 * \brief Create CompData* (%cid) for PDF from a file (%fname).
 * <pre>
 * Arg #1 is expected to be a string (fname).
 * Arg #2 is expected to be a Pix* (pix).
 * Arg #3 is expected to be a l_int32 (quality).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
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
        return ll_push_nil(L);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Flate compress data from a file (%fname) to a CompData* (%cid).
 * <pre>
 * Arg #1 is expected to be a string (fname).
 * Arg #2 is expected to be a l_int32 (ascii85flag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
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
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
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
        return ll_push_nil(L);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Generate CompData* (%cid) from a Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (type).
 * Arg #3 is expected to be a l_int32 (quality).
 * Arg #4 is expected to be a l_int32 (ascii85).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
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
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Generate CompData* (%cid) from a file (%filein) using flate compression.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CompData* (cid).
 * Arg #2 is expected to be a string (filein).
 * Arg #3 is expected to be a l_float32 (xpt).
 * Arg #4 is expected to be a l_float32 (ypt).
 * Arg #5 is expected to be a l_float32 (wpt).
 * Arg #6 is expected to be a l_float32 (hpt).
 * Arg #7 is expected to be a l_int32 (pageno).
 * Arg #8 is expected to be a l_int32 (endpage).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring on the Lua stack
 */
static int
GenerateFlatePS(lua_State *L)
{
    LL_FUNC("GenerateFlatePS");
    CompData *cid = ll_check_CompData(_fun, L, 1);
    const char *filein = ll_check_string(_fun, L, 2);
    l_float32 xpt = ll_check_l_float32(_fun, L, 3);
    l_float32 ypt = ll_check_l_float32(_fun, L, 4);
    l_float32 wpt = ll_check_l_float32(_fun, L, 5);
    l_float32 hpt = ll_check_l_float32(_fun, L, 6);
    l_int32 pageno = ll_check_l_int32(_fun, L, 7);
    l_int32 endpage = ll_check_l_int32(_fun, L, 8);
    char *str = generateFlatePS(filein, cid, xpt, ypt, wpt, hpt, pageno, endpage);
    ll_push_string(_fun, L, str);
    ll_free(str);
    return 1;
}

/**
 * \brief Generate CompData* (%cid) from a file (%filein) using G4 compression.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CompData* (cid).
 * Arg #2 is expected to be a string (filein).
 * Arg #3 is expected to be a l_float32 (xpt).
 * Arg #4 is expected to be a l_float32 (ypt).
 * Arg #5 is expected to be a l_float32 (wpt).
 * Arg #6 is expected to be a l_float32 (hpt).
 * Arg #7 is expected to be a l_int32 (maskflag).
 * Arg #8 is expected to be a l_int32 (pageno).
 * Arg #9 is expected to be a l_int32 (endpage).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring on the Lua stack
 */
static int
GenerateG4PS(lua_State *L)
{
    LL_FUNC("GenerateG4PS");
    CompData *cid = ll_check_CompData(_fun, L, 1);
    const char *filein = ll_check_string(_fun, L, 2);
    l_float32 xpt = ll_check_l_float32(_fun, L, 3);
    l_float32 ypt = ll_check_l_float32(_fun, L, 4);
    l_float32 wpt = ll_check_l_float32(_fun, L, 5);
    l_float32 hpt = ll_check_l_float32(_fun, L, 6);
    l_int32 maskflag = ll_check_l_int32(_fun, L, 7);
    l_int32 pageno = ll_check_l_int32(_fun, L, 8);
    l_int32 endpage = ll_check_l_int32(_fun, L, 9);
    char *str = generateG4PS(filein, cid, xpt, ypt, wpt, hpt, maskflag, pageno, endpage);
    ll_push_string(_fun, L, str);
    ll_free(str);
    return 1;
}

/**
 * \brief Generate CompData* (%cid) from a file (%filein) using JPEG compression.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CompData* (cid).
 * Arg #2 is expected to be a string (filein).
 * Arg #3 is expected to be a l_float32 (xpt).
 * Arg #4 is expected to be a l_float32 (ypt).
 * Arg #5 is expected to be a l_float32 (wpt).
 * Arg #6 is expected to be a l_float32 (hpt).
 * Arg #7 is expected to be a l_int32 (pageno).
 * Arg #8 is expected to be a l_int32 (endpage).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring on the Lua stack
 */
static int
GenerateJpegPS(lua_State *L)
{
    LL_FUNC("GenerateJpegPS");
    CompData *cid = ll_check_CompData(_fun, L, 1);
    const char *filein = ll_check_string(_fun, L, 2);
    l_float32 xpt = ll_check_l_float32(_fun, L, 3);
    l_float32 ypt = ll_check_l_float32(_fun, L, 4);
    l_float32 wpt = ll_check_l_float32(_fun, L, 5);
    l_float32 hpt = ll_check_l_float32(_fun, L, 6);
    l_int32 pageno = ll_check_l_int32(_fun, L, 7);
    l_int32 endpage = ll_check_l_int32(_fun, L, 8);
    char *str = generateJpegPS(filein, cid, xpt, ypt, wpt, hpt, pageno, endpage);
    ll_push_string(_fun, L, str);
    ll_free(str);
    return 1;
}
/**
 * \brief JPEG compress data from a file (%fname) to a CompData* (%cid).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (fname).
 * Arg #2 is expected to be a l_int32 (ascii85flag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
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
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
 */
static int
JpegDataMem(lua_State *L)
{
    LL_FUNC("JpegDataMem");
    size_t nbytes = 0;
    const char *str = ll_check_lstring(_fun, L, 1, &nbytes);
    l_int32 ascii85flag = ll_check_boolean(_fun, L, 2);
    l_uint8* data = reinterpret_cast<l_uint8 *>(reinterpret_cast<l_intptr_t>(str));
    CompData *cid = l_generateJpegDataMem(data, nbytes, ascii85flag);
    return ll_push_CompData(_fun, L, cid);
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_COMPDATA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CompData* contained in the user data
 */
CompData *
ll_check_CompData(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<CompData>(_fun, L, arg, LL_COMPDATA);
}
/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CompData* contained in the user data
 */
CompData *
ll_check_CompData_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_CompData(_fun, L, arg);
}
/**
 * \brief Push BMF user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cdata pointer to the L_CompData
 * \return 1 CompData* on the Lua stack
 */
int
ll_push_CompData(const char *_fun, lua_State *L, CompData *cdata)
{
    if (!cdata)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_COMPDATA, cdata);
}
/**
 * \brief Generate and push a new CompData*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 CompData* on the Lua stack
 */
int
ll_new_CompData(lua_State *L)
{
    return Generate(L);
}
/**
 * \brief Register the BMF methods and functions in the LL_COMPDATA meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_CompData(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},
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
        {"GenerateFlatePS",     GenerateFlatePS},
        {"GenerateG4PS",        GenerateG4PS},
        {"GenerateJpegPS",      GenerateJpegPS},
        {"JpegData",            JpegData},
        {"JpegDataMem",         JpegDataMem},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Generate);
    lua_setglobal(L, LL_COMPDATA);
    return ll_register_class(L, LL_COMPDATA, methods, functions);
}
