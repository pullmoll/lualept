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
 *  Lua class PIXA
 *
 *====================================================================*/

/**
 * \brief Create a new Pixa*
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    FUNC(LL_PIXA ".Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    Pixa *pixa = pixaCreate(n);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Destroy a Pixa*
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    FUNC(LL_PIXA ".Destroy");
    Pixa **ppixa = reinterpret_cast<Pixa **>(ll_check_udata(_fun, L, 1, LL_PIXA));
    DBG(LOG_DESTROY, "%s: '%s' ppa=%p pa=%p\n", _fun,
        LL_PIXA, ppixa, *ppixa);
    pixaDestroy(ppixa);
    *ppixa = nullptr;
    return 0;
}

/**
 * \brief Copy a Pixa* (%pixas)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is an optional string defining the storage flags (copy, clone, copy_clone).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    FUNC(LL_PIXA ".Copy");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Pixa *pixa = pixaCopy(pixas, copyflag);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Add a Pix* (%pix) to a Pixa* (%pixa)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a Pix*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPix(lua_State *L)
{
    FUNC(LL_PIXA ".AddPix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    lua_pushboolean(L, 0 == pixaAddPix(pixa, pix, flag));
    return 1;
}

/**
 * \brief Get count for a Pixa* (%pixa)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    FUNC(LL_PIXA ".GetCount");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    lua_pushinteger(L, pixaGetCount(pixa));
    return 1;
}

/**
 * \brief Get box geometry for a Pix* from a Pixa* (%pixa) at index (%idx)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack: x, y, w, h
 */
static int
GetBoxGeometry(lua_State *L)
{
    FUNC(LL_PIXA ".GetBoxGeometry");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    l_int32 x, y, w, h;
    if (pixaGetBoxGeometry(pixa, idx, &x, &y, &w, &h))
        return ll_push_nil(L);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 4;
}

/**
 * \brief Replace the Pix* in a Pixa* at index %idx
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix* (pixs).
 * Arg #4 is optional and, if given, expected to be a Box* (boxs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplacePix(lua_State *L)
{
    FUNC(LL_PIXA ".ReplacePix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    Box *boxs = ll_check_Box_opt(_fun, L, 4);
    Pix *pix = pixClone(pixs);
    Box *box = boxs ? boxClone(boxs) : nullptr;
    lua_pushboolean(L, pix && 0 == pixaReplacePix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief Insert the Pix* (%pixs) in a Pixa* (%pixa) at index (%idx)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix* (%pixs).
 * Arg #4 is optional and, if given, expected to be a Box* (%boxs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertPix(lua_State *L)
{
    FUNC(LL_PIXA ".InsertPix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    Box *boxs = ll_check_Box_opt(_fun, L, 4);
    Pix *pix = pixClone(pixs);
    Box *box = boxs ? boxClone(boxs) : nullptr;
    lua_pushboolean(L, pix && 0 == pixaInsertPix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief Remove the Pix* from a Pixa* (%pixa) at index (%idx)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemovePix(lua_State *L)
{
    FUNC(LL_PIXA ".RemovePix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    lua_pushboolean(L, 0 == pixaRemovePix(pixa, idx));
    return 1;
}

/**
 * \brief Remove the Pix* (%pix) from a Pixa* (%pixa) at index (%idx) and return it and its box
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (%idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Pix* (pix) and one Box* (box) on the Lua stack
 */
static int
RemovePixAndSave(lua_State *L)
{
    FUNC(LL_PIXA ".RemovePixAndSave");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pix = nullptr;
    Box *box = nullptr;
    if (pixaRemovePixAndSave(pixa, idx, &pix, &box))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pix) + ll_push_Box(_fun, L, box);
}

/**
 * \brief Join Pixa* (%pixas) to Pixa* (%pixad)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixad).
 * Arg #2 is expected to be another Pixa* (pixas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Join(lua_State *L)
{
    FUNC(LL_PIXA ".Join");
    Pixa *pixad = ll_check_Pixa(_fun, L, 1);
    Pixa *pixas = ll_check_Pixa(_fun, L, 2);
    l_int32 istart = ll_check_l_int32_default(_fun, L, 3, 1);
    l_int32 iend = ll_check_l_int32_default(_fun, L, 3, pixaGetCount(pixas));
    lua_pushboolean(L, 0 == pixaJoin(pixad, pixas, istart, iend));
    return 1;
}

/**
 * \brief Interleave two Pixa* (%pixa1, %pixa2)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa1).
 * Arg #2 is expected to be another Pixa* (pixa2).
 * Arg #3 is an optional string defining the storage flags (copy, clone, copy_clone).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
Interleave(lua_State *L)
{
    FUNC(LL_PIXA ".Interleave");
    Pixa *pixa1 = ll_check_Pixa(_fun, L, 1);
    Pixa *pixa2 = ll_check_Pixa(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    Pixa *pixa = pixaInterleave(pixa1, pixa2, copyflag);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Clear the Pixa* (%pixa)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    FUNC(LL_PIXA ".Clear");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    lua_pushboolean(L, 0 == pixaClear(pixa));
    return 1;
}

/**
 * \brief Get pixel aligned statistics for Pixa* (%pixa)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GetAlignedStats(lua_State *L)
{
    FUNC(LL_PIXA ".GetAlignedStats");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    l_int32 nbins = ll_check_l_int32_default(_fun, L, 3, 2);
    l_int32 thresh = ll_check_l_int32_default(_fun, L, 4, 0);
    Pix *pix = pixaGetAlignedStats(pixa, type, nbins, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Read a Pixa* from an external file
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
Read(lua_State *L)
{
    FUNC(LL_PIXA ".Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Pixa *pixa = pixaRead(filename);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Read a Pixa* (%pixa) from a number of external files
 * <pre>
 * Arg #1 is expected to be a string containing the directory (dirname).
 * Arg #2 is expected to be a string (substr).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
ReadFiles(lua_State *L)
{
    FUNC(LL_PIXA ".ReadFiles");
    const char *dirname = ll_check_string(_fun, L, 1);
    const char *substr = ll_check_string(_fun, L, 2);
    Pixa *pixa = pixaReadFiles(dirname, substr);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Read a Pixa* from a Lua io stream (%stream)
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    FUNC(LL_PIXA ".ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pixa *pixa = pixaReadStream(stream->f);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Read a Pixa* from a Lua string (%data)
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    FUNC(LL_PIXA ".ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Pixa *pixa = pixaReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Write the Pixa* (%pixa) to an external file (%filename)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * Arg #2 is expected to be string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    FUNC(LL_PIXA ".Write");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    lua_pushboolean(L, 0 == pixaWrite(filename, pixa));
    return 1;
}

/**
 * \brief Write the Pixa* to an Lua io stream (%stream)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    FUNC(LL_PIXA ".WriteStream");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    lua_pushboolean(L, 0 == pixaWriteStream(stream->f, pixa));
    return 1;
}

/**
 * \brief Write the Pixa* (%pixa) to memory and return it as a Lua string
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    FUNC(LL_PIXA ".WriteMem");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixaWriteMem(&data, &size, pixa))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    LEPT_FREE(data);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_PIXA
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIXA contained in the user data
 */
Pixa *
ll_check_Pixa(const char *_fun, lua_State *L, int arg)
{
    return *(reinterpret_cast<Pixa **>(ll_check_udata(_fun, L, arg, LL_PIXA)));
}

/**
 * \brief Optionally expect a LL_PIXA at index %arg on the Lua stack
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pixa* contained in the user data
 */
Pixa *
ll_check_Pixa_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Pixa(_fun, L, arg);
}

/**
 * \brief Push Pixa* to the Lua stack and set its meta table
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param pixa pointer to the PIXA
 * \return 1 Pixa* on the Lua stack
 */
int
ll_push_Pixa(const char *_fun, lua_State *L, Pixa *pixa)
{
    if (!pixa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_PIXA, pixa);
}

/**
 * \brief Create a new Pixa*
 * \param L pointer to the lua_State
 * \return 1 Pixa* on the Lua stack
 */
int
ll_new_Pixa(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Pixa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},           /* garbage collector */
        {"__new",               Create},            /* new Pixa */
        {"__len",               GetCount},          /* #pa */
        {"GetCount",            GetCount},
        {"Copy",                Copy},
        {"Destroy",             Destroy},
        {"AddPix",              AddPix},
        {"GetBoxGeometry",      GetBoxGeometry},
        {"ReplacePix",          ReplacePix},
        {"InsertPix",           InsertPix},
        {"RemovePix",           RemovePix},
        {"RemovePixAndSave",    RemovePixAndSave},
        {"TakePix",             RemovePixAndSave},  /* alias name */
        {"Join",                Join},
        {"Interleave",          Interleave},
        {"Clear",               Clear},
        {"GetAlignedStats",     GetAlignedStats},
        {"Write",               Write},
        {"WriteStream",         WriteStream},
        {"WriteMem",            WriteMem},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        {"Create",              Create},
        {"Read",                Read},
        {"ReadFiles",           ReadFiles},
        {"ReadStream",          ReadStream},
        {"ReadMem",             ReadMem},
        LUA_SENTINEL
    };

    return ll_register_class(L, LL_PIXA, methods, functions);
}
