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
 *  Lua class Dewarpa
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_DEWARPA */
#define LL_FUNC(x) FUNC(LL_DEWARPA "." x)

/**
 * \brief Destroy a Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 void on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Dewarpa **pdewa = ll_check_udata<Dewarpa>(_fun, L, 1, LL_DEWARPA);
    Dewarpa *dewa = *pdewa;
    DBG(LOG_DESTROY, "%s: '%s' pdewa=%p dewa=%p\n", _fun,
        LL_DEWARP, pdewa, dewa);
    dewarpaDestroy(&dewa);
    *pdewa = nullptr;
    return 0;
}

/**
 * \brief Create a new Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (nptrs).
 * Arg #2 is expected to be a l_int32 (sampling).
 * Arg #3 is expected to be a l_int32 (redfactor).
 * Arg #4 is expected to be a l_int32 (minlines).
 * Arg #5 is expected to be a l_int32 (maxdist).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 nptrs = ll_check_l_int32(_fun, L, 1);
    l_int32 sampling = ll_check_l_int32(_fun, L, 2);
    l_int32 redfactor = ll_check_l_int32(_fun, L, 3);
    l_int32 minlines = ll_check_l_int32(_fun, L, 4);
    l_int32 maxdist = ll_check_l_int32(_fun, L, 5);
    Dewarpa *dewa = dewarpaCreate(nptrs, sampling, redfactor, minlines, maxdist);
    return ll_push_Dewarpa(_fun, L, dewa);
}

/**
 * \brief Printable string for a Dewarpa*.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!dewa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 LL_DEWARPA ": %p\n",
                 reinterpret_cast<void *>(dewa));
        /* TODO: more info */
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Apply disparity for a Dewarpa* (%dewa) and return the Pix* (%pixd).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (pageno).
 * Arg #3 is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a l_int32 (grayin).
 * Arg #5 is expected to be a l_int32 (x).
 * Arg #6 is expected to be a l_int32 (y).
 * Arg #7 is expected to be a string (debugfile).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (%pixd) on the Lua stack
 */
static int
ApplyDisparity(lua_State *L)
{
    LL_FUNC("ApplyDisparity");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    l_int32 grayin = ll_check_l_int32(_fun, L, 4);
    l_int32 x = ll_check_l_int32(_fun, L, 5);
    l_int32 y = ll_check_l_int32(_fun, L, 6);
    const char *debugfile = ll_check_string(_fun, L, 7);
    Pix *pixd = nullptr;
    if (dewarpaApplyDisparity(dewa, pageno, pixs, grayin, x, y, &pixd, debugfile))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pixd);
}

/**
 * \brief Apply disparity for a Dewarpa* (%dewa) and return the Boxa* (%boxad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (pageno).
 * Arg #3 is expected to be a Pix* (pixs).
 * Arg #4 is expected to be a Boxa* (boxas).
 * Arg #5 is expected to be a l_int32 (mapdir).
 * Arg #6 is expected to be a l_int32 (x).
 * Arg #7 is expected to be a l_int32 (y).
 * Arg #8 is expected to be a string (debugfile).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* (%boxad) on the Lua stack
 */
static int
ApplyDisparityBoxa(lua_State *L)
{
    LL_FUNC("ApplyDisparityBoxa");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    Boxa *boxas = ll_check_Boxa(_fun, L, 4);
    l_int32 mapdir = ll_check_l_int32(_fun, L, 5);
    l_int32 x = ll_check_l_int32(_fun, L, 6);
    l_int32 y = ll_check_l_int32(_fun, L, 7);
    const char *debugfile = ll_check_string(_fun, L, 8);
    Boxa *boxad = nullptr;
    if (dewarpaApplyDisparityBoxa(dewa, pageno, pixs, boxas, mapdir, x, y, &boxad, debugfile))
        return ll_push_nil(L);
    return ll_push_Boxa(_fun, L, boxad);
}

/**
 * \brief Destroy a Dewarp* for page %pageno in the Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (pageno).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
DestroyDewarp(lua_State *L)
{
    LL_FUNC("DestroyDewarp");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    ll_push_bool(_fun, L, 0 == dewarpaDestroyDewarp(dewa, pageno));
}

/**
 * \brief Get the Dewarp* for page %pageno from the Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (index).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarp * on the Lua stack
 */
static int
GetDewarp(lua_State *L)
{
    LL_FUNC("GetDewarp");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 index = ll_check_l_int32(_fun, L, 2);
    Dewarp *dew = dewarpaGetDewarp(dewa, index);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Info about Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Info(lua_State *L)
{
    LL_FUNC("Info");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dewarpaInfo(stream->f, dewa));
}

/**
 * \brief Insert a Dewarp* (%dew) into Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a Dewarp* (dew).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
InsertDewarp(lua_State *L)
{
    LL_FUNC("InsertDewarp");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    Dewarp *dew = ll_check_Dewarp(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dewarpaInsertDewarp(dewa, dew));
}

/**
 * \brief Insert reference models into Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a boolean (notests).
 * Arg #3 is expected to be a boolean (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertRefModels(lua_State *L)
{
    LL_FUNC("InsertRefModels");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 notests = ll_check_boolean(_fun, L, 2);
    l_int32 debug = ll_check_boolean(_fun, L, 3);
    return ll_push_bool(_fun, L, 0 == dewarpaInsertRefModels(dewa, notests, debug));
}

/**
 * \brief List pages for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ListPages(lua_State *L)
{
    LL_FUNC("ListPages");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 res = dewarpaListPages(dewa);
    lua_pushinteger(L, res);
    return 1;
}

/**
 * \brief Get model stats for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 6 integers on the Lua stack
 */
static int
ModelStats(lua_State *L)
{
    LL_FUNC("ModelStats");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 nnone = 0;
    l_int32 nvsuccess = 0;
    l_int32 nvvalid = 0;
    l_int32 nhsuccess = 0;
    l_int32 nhvalid = 0;
    l_int32 nref = 0;
    if (dewarpaModelStats(dewa, &nnone, &nvsuccess, &nvvalid, &nhsuccess, &nhvalid, &nref))
        return ll_push_nil(L);
    lua_pushinteger(L, nnone);
    lua_pushinteger(L, nvsuccess);
    lua_pushinteger(L, nvvalid);
    lua_pushinteger(L, nhsuccess);
    lua_pushinteger(L, nhvalid);
    lua_pushinteger(L, nref);
    return 6;
}

/**
 * \brief Get model status for a page (%pageno) in Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (pageno).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ModelStatus(lua_State *L)
{
    LL_FUNC("ModelStatus");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    l_int32 vsuccess = 0;
    l_int32 hsuccess = 0;
    if (dewarpaModelStatus(dewa, pageno, &vsuccess, &hsuccess))
        return ll_push_nil(L);
    lua_pushinteger(L, vsuccess);
    lua_pushinteger(L, hsuccess);
    return 2;
}

/**
 * \brief Read a Dewarpa* from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa * on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Dewarpa * result = dewarpaRead(filename);
    return ll_push_Dewarpa(_fun, L, result);
}

/**
 * \brief Read a Dewarpa* from a lstring (%str, %size).
 * <pre>
 * Arg #1 is expected to be a lstring (str).
 * Arg #2 is expected to be a size_t (size).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa * on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size = 0;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    l_uint8 *data = reinterpret_cast<l_uint8 *>(reinterpret_cast<l_intptr_t>(str));
    Dewarpa *dewa = dewarpaReadMem(data, size);
    return ll_push_Dewarpa(_fun, L, dewa);
}

/**
 * \brief Read a Dewarpa* from a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (%stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Dewarpa *dewa = dewarpaReadStream(stream->f);
    return ll_push_Dewarpa(_fun, L, dewa);
}

/**
 * \brief Restore models for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RestoreModels(lua_State *L)
{
    LL_FUNC("RestoreModels");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    ll_push_bool(_fun, L, 0 == dewarpaRestoreModels(dewa));
}

/**
 * \brief Set check columns for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (check_columns).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetCheckColumns(lua_State *L)
{
    LL_FUNC("SetCheckColumns");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 check_columns = ll_check_l_int32(_fun, L, 2);
    ll_push_bool(_fun, L, 0 == dewarpaSetCheckColumns(dewa, check_columns));
}

/**
 * \brief Set curvatures for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (max_linecurv).
 * Arg #3 is expected to be a l_int32 (min_diff_linecurv).
 * Arg #4 is expected to be a l_int32 (max_diff_linecurv).
 * Arg #5 is expected to be a l_int32 (max_edgecurv).
 * Arg #6 is expected to be a l_int32 (max_diff_edgecurv).
 * Arg #7 is expected to be a l_int32 (max_edgeslope).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetCurvatures(lua_State *L)
{
    LL_FUNC("SetCurvatures");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 max_linecurv = ll_check_l_int32(_fun, L, 2);
    l_int32 min_diff_linecurv = ll_check_l_int32(_fun, L, 3);
    l_int32 max_diff_linecurv = ll_check_l_int32(_fun, L, 4);
    l_int32 max_edgecurv = ll_check_l_int32(_fun, L, 5);
    l_int32 max_diff_edgecurv = ll_check_l_int32(_fun, L, 6);
    l_int32 max_edgeslope = ll_check_l_int32(_fun, L, 7);
    ll_push_bool(_fun, L, 0 == dewarpaSetCurvatures(dewa,
                                              max_linecurv, min_diff_linecurv, max_diff_linecurv,
                                              max_edgecurv, max_diff_edgecurv, max_edgeslope));
}

/**
 * \brief Set max distance for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (maxdist).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetMaxDistance(lua_State *L)
{
    LL_FUNC("SetMaxDistance");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 maxdist = ll_check_l_int32(_fun, L, 2);
    ll_push_bool(_fun, L, 0 == dewarpaSetMaxDistance(dewa, maxdist));
}

/**
 * \brief Set valid models for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (notests).
 * Arg #3 is expected to be a l_int32 (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
SetValidModels(lua_State *L)
{
    LL_FUNC("SetValidModels");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 notests = ll_check_boolean(_fun, L, 2);
    l_int32 debug = ll_check_boolean(_fun, L, 3);
    ll_push_bool(_fun, L, 0 == dewarpaSetValidModels(dewa, notests, debug));
}

/**
 * \brief Show arrays for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_float32 (scalefact).
 * Arg #3 is expected to be a l_int32 (first).
 * Arg #4 is expected to be a l_int32 (last).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
ShowArrays(lua_State *L)
{
    LL_FUNC("ShowArrays");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_float32 scalefact = ll_check_l_float32(_fun, L, 2);
    l_int32 first = ll_check_l_int32(_fun, L, 3);
    l_int32 last = ll_check_l_int32(_fun, L, 4);
    return ll_push_bool(_fun, L, 0 == dewarpaShowArrays(dewa, scalefact, first, last));
}

/**
 * \brief Strip reference models from Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
StripRefModels(lua_State *L)
{
    LL_FUNC("StripRefModels");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    return ll_push_bool(_fun, L, 0 == dewarpaStripRefModels(dewa));
}

/**
 * \brief Set use both arrays for Dewarpa* (%dewa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a l_int32 (useboth).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
UseBothArrays(lua_State *L)
{
    LL_FUNC("UseBothArrays");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    l_int32 useboth = ll_check_boolean(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dewarpaUseBothArrays(dewa, useboth));
}

/**
 * \brief Write Dewarpa* (%dewa) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dewarpaWrite(filename, dewa));
}

/**
 * \brief Write Dewarpa* (%dewa) to a lstring (%data, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    size_t size = 0;
    l_uint8 *data = nullptr;
    if (dewarpaWriteMem(&data, &size, dewa))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write Dewarpa* (%dewa) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarpa* (dewa).
 * Arg #2 is expected to be a luaL_Stream* (%stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Dewarpa *dewa = ll_check_Dewarpa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dewarpaWriteStream(stream->f, dewa));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixaComp* (pixac).
 * Arg #2 is expected to be a l_int32 (useboth).
 * Arg #3 is expected to be a l_int32 (sampling).
 * Arg #4 is expected to be a l_int32 (minlines).
 * Arg #5 is expected to be a l_int32 (maxdist).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarpa * on the Lua stack
 */
static int
CreateFromPixacomp(lua_State *L)
{
    LL_FUNC("CreateFromPixacomp");
    PixaComp *pixac = ll_check_PixaComp(_fun, L, 1);
    l_int32 useboth = ll_check_l_int32(_fun, L, 2);
    l_int32 sampling = ll_check_l_int32(_fun, L, 3);
    l_int32 minlines = ll_check_l_int32(_fun, L, 4);
    l_int32 maxdist = ll_check_l_int32(_fun, L, 5);
    Dewarpa * result = dewarpaCreateFromPixacomp(pixac, useboth, sampling, minlines, maxdist);
    return ll_push_Dewarpa(_fun, L, result);
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_DEWARPA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dewarpa* contained in the user data
 */
Dewarpa *
ll_check_Dewarpa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Dewarpa>(_fun, L, arg, LL_DEWARPA);
}

/**
 * \brief Optionally expect a LL_DEWARPA at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dewarpa* contained in the user data
 */
Dewarpa *
ll_check_Dewarpa_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Dewarpa(_fun, L, arg);
}
/**
 * \brief Push BOX user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param dew pointer to the Dewarpa
 * \return 1 Dewarpa* on the Lua stack
 */
int
ll_push_Dewarpa(const char *_fun, lua_State *L, Dewarpa *dew)
{
    if (!dew)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_DEWARPA, dew);
}
/**
 * \brief Create and push a new Dewarpa*.
 * \param L pointer to the lua_State
 * \return 1 Dewarpa* on the Lua stack
 */
int
ll_new_Dewarpa(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the Dewarpa* methods and functions in the LL_DEWARP meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Dewarpa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},   /* garbage collector */
        {"__new",                   Create},    /* new Dewarpa */
        {"__tostring",              toString},
        {"ApplyDisparity",          ApplyDisparity},
        {"ApplyDisparityBoxa",      ApplyDisparityBoxa},
        {"Create",                  Create},
        {"CreateFromPixacomp",      CreateFromPixacomp},
        {"Destroy",                 Destroy},
        {"DestroyDewarp",           DestroyDewarp},
        {"GetDewarp",               GetDewarp},
        {"Info",                    Info},
        {"InsertDewarp",            InsertDewarp},
        {"InsertRefModels",         InsertRefModels},
        {"ListPages",               ListPages},
        {"ModelStats",              ModelStats},
        {"ModelStatus",             ModelStatus},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RestoreModels",           RestoreModels},
        {"SetCheckColumns",         SetCheckColumns},
        {"SetCurvatures",           SetCurvatures},
        {"SetMaxDistance",          SetMaxDistance},
        {"SetValidModels",          SetValidModels},
        {"ShowArrays",              ShowArrays},
        {"StripRefModels",          StripRefModels},
        {"UseBothArrays",           UseBothArrays},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_DEWARPA);
    return ll_register_class(L, LL_DEWARPA, methods, functions);
}
