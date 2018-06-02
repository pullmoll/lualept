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
 * \file llboxaa.cpp
 * \class Boxaa
 *
 * An array of Boxa.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_BOXAA

/** Define a function's name (_fun) with prefix Boxaa */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Boxaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Boxaa **pbaa = ll_check_udata<Boxaa>(_fun, L, 1, TNAME);
    Boxaa *baa = *pbaa;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d\n", _fun,
        TNAME,
        "pbaa", reinterpret_cast<void *>(pbaa),
        "baa", reinterpret_cast<void *>(baa),
        "count", boxaaGetCount(baa));
    boxaaDestroy(&baa);
    *pbaa = nullptr;
    return 0;
}

/**
 * \brief Get count for a Boxaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, boxaaGetCount(boxaa));
    return 1;
}

/**
 * \brief Printable string for a Boxaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    luaL_Buffer B;
    l_int32 i, j, x, y, w, h;

    luaL_buffinit(L, &B);
    if (!boxaa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 TNAME ": %p",
                 reinterpret_cast<void *>(boxaa));
        luaL_addstring(&B, str);
        for (i = 0; i < boxaaGetCount(boxaa); i++) {
            Boxa *boxa = boxaaGetBoxa(boxaa, i, L_CLONE);
            snprintf(str, sizeof(str),
                     "\n    %d = {", i+1);
            luaL_addstring(&B, str);
            for (j = 0; j < boxaGetCount(boxa); j++) {
                boxaGetBoxGeometry(boxa, j, &x, &y, &w, &h);
                snprintf(str, sizeof(str),
                         "\n       %d = { x = %d, y = %d, w = %d, h = %d }",
                         j+1, x, y, w, h);
                luaL_addstring(&B, str);
            }
            luaL_addstring(&B, "\n    }");
            boxaDestroy(&boxa);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Add a Box* to a Boxaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a Boxa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddBoxa(lua_State *L)
{
    LL_FUNC("AddBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    Boxa *boxa = ll_check_Boxa(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == boxaaAddBoxa(boxaa, boxa, flag));
}

/**
 * \brief Copy a Boxaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baas).
 * Arg #2 is an optional string defining the storage flags (copyflag).
 *
 * Leptonica's Notes:
 *      (1) L_COPY makes a copy of each boxa in baas.
 *          L_CLONE makes a clone of each boxa in baas.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Boxaa *baas = ll_check_Boxaa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Boxaa *baa = boxaaCopy(baas, copyflag);
    return ll_push_Boxaa(_fun, L, baa);
}

/**
 * \brief Create a new Boxaa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Boxaa *boxaa = boxaaCreate(n);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Extend a Boxaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArray(lua_State *L)
{
    LL_FUNC("ExtendArray");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == boxaaExtendArray(boxaa));
}

/**
 * \brief Extend a Boxaa* to a given size %n.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (n).
 *
 * Leptonica's Notes:
 *      (1) If necessary, reallocs the boxa ptr array to %size.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArrayToSize(lua_State *L)
{
    LL_FUNC("ExtendArrayToSize");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 size = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaaExtendArrayToSize(boxaa, size));
}

/**
 * \brief Aligned flatten the Boxaa* to a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (boxaa).
 * Arg #2 is expected to be a l_int32 (num).
 * Arg #3 is expected to be a string describing the copy flag (copyflag).
 * Arg #3 is an optional Box* (fillerbox).
 *
 * Leptonica's Notes:
 *      (1) This 'flattens' the baa to a boxa, taking the first %num
 *          boxes from each boxa.
 *      (2) In each boxa, if there are less than %num boxes, we preserve
 *          the alignment between the input baa and the output boxa
 *          by inserting one or more fillerbox(es) or, if %fillerbox == NULL,
 *          one or more invalid placeholder boxes.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Boxa* on the Lua stack
 */
static int
FlattenAligned(lua_State *L)
{
    LL_FUNC("FlattenAligned");
    Boxaa *baa = ll_check_Boxaa(_fun, L, 1);
    l_int32 num = ll_check_l_int32(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Box *fillerbox = ll_opt_Box(_fun, L, 4);
    Boxa *ba = boxaaFlattenAligned(baa, num, fillerbox, copyflag);
    return ll_push_Boxa(_fun, L, ba);
}

/**
 * \brief Flatten the Boxaa* to a Boxa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (boxaa).
 * Arg #2 is expected to be a string describing the copy flag (copyflag).
 *
 * Leptonica's Notes:
 *      (1) This 'flattens' the baa to a boxa, taking the boxes in
 *          order in the first boxa, then the second, etc.
 *      (2) If a boxa is empty, we generate an invalid, placeholder box
 *          of zero size.  This is useful when converting from a baa
 *          where each boxa has either 0 or 1 boxes, and it is necessary
 *          to maintain a 1:1 correspondence between the initial
 *          boxa array and the resulting box array.
 *      (3) If &naindex is defined, we generate a Numa that gives, for
 *          each box in the baa, the index of the boxa to which it belongs.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FlattenToBoxa(lua_State *L)
{
    LL_FUNC("FlattenToBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Numa *naindex = nullptr;
    Boxa *boxa = boxaaFlattenToBoxa(boxaa, &naindex, copyflag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Get Box* from a Boxaa* at index %iboxa and %ibox.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (iboxa).
 * Arg #2 is expected to be a l_int32 (ibox).
 * Arg #3 is an optional string defining the storage flags (copy, clone)..
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBox(lua_State *L)
{
    LL_FUNC("GetBox");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 iboxa = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    l_int32 ibox = ll_check_index(_fun, L, 3, INT32_MAX);
    l_int32 flag = ll_check_access_storage(_fun, L, 4, L_COPY);
    Box *box = boxaaGetBox(boxaa, iboxa, ibox, flag);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Copy a Boxaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBoxCount(lua_State *L)
{
    LL_FUNC("GetBoxCount");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    ll_push_l_int32(_fun, L, boxaaGetBoxCount(boxaa));
    return 1;
}

/**
 * \brief Get Boxa* from a Boxaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is an optional string defining the storage flags (copy, clone)..
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetBoxa(lua_State *L)
{
    LL_FUNC("GetBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    Boxa *boxa = boxaaGetBoxa(boxaa, idx, flag);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Insert the Boxa* in a Boxaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Boxa* user data.
 *
 * Leptonica's Notes:
 *      (1) This shifts boxa[i] --> boxa[i + 1] for all i >= index,
 *          and then inserts boxa as boxa[index].
 *      (2) To insert at the beginning of the array, set index = 0.
 *      (3) To append to the array, it's easier to use boxaaAddBoxa().
 *      (4) This should not be used repeatedly to insert into large arrays,
 *          because the function is O(n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertBoxa(lua_State *L)
{
    LL_FUNC("InsertBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    Boxa *boxas = ll_check_Boxa(_fun, L, 3);
    Boxa *boxa = boxaCopy(boxas, L_CLONE);
    return ll_push_boolean(_fun, L, boxa && 0 == boxaaInsertBoxa(boxaa, idx, boxa));
}

/**
 * \brief Join Boxaa* (%boxaas) with Boxa* (%boxaad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (boxaad).
 * Arg #2 is expected to be another Box* (boxaas).
 * Arg #3 is an optional l_int32 (istart).
 * Arg #4 is an optional l_int32 (iend).
 *
 * Leptonica's Notes:
 *      (1) This appends a clone of each indicated boxa in baas to baad
 *      (2) istart < 0 is taken to mean 'read from the start' (istart = 0)
 *      (3) iend < 0 means 'read to the end'
 *      (4) if baas == NULL, this is a no-op.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 boolean and Numa* on the Lua stack
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    Boxaa *boxaad = ll_check_Boxaa(_fun, L, 1);
    Boxaa *boxaas = ll_check_Boxaa(_fun, L, 2);
    l_int32 istart = ll_check_index(_fun, L, 3, 1);
    l_int32 iend = ll_check_index(_fun, L, 3, boxaaGetCount(boxaas));
    return ll_push_boolean(_fun, L, 0 == boxaaJoin(boxaad, boxaas, istart, iend));
}

/**
 * \brief Read a Boxaa* (%boxaa) from a file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Boxaa *boxaa = boxaaRead(filename);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Read a Boxaa* (%boxaa) from memory (%data).
 * <pre>
 * Arg #1 is expected to be a pstring (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
    Boxaa *boxaa = boxaaReadMem(data, size);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Read a Boxaa* (%boxaa) from a stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Box* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Boxaa *boxaa = boxaaReadStream(stream->f);
    return ll_push_Boxaa(_fun, L, boxaa);
}

/**
 * \brief Reomve the Boxa* from a Boxaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (%idx).
 *
 * Leptonica's Notes:
 *      (1) This removes boxa[index] and then shifts
 *          boxa[i] --> boxa[i - 1] for all i > index.
 *      (2) The removed boxaa is destroyed.
 *      (2) This should not be used repeatedly on large arrays,
 *          because the function is O(n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemoveBoxa(lua_State *L)
{
    LL_FUNC("RemoveBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    return ll_push_boolean(_fun, L, 0 == boxaaRemoveBoxa(boxaa, idx));
}

/**
 * \brief Replace the Box* in a Boxaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Boxaa* (baa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Boxa* user data.
 *
 * Leptonica's Notes:
 *      (1) Any existing boxa is destroyed, and the input one
 *          is inserted in its place.
 *      (2) If the index is invalid, return 1 (error)
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceBoxa(lua_State *L)
{
    LL_FUNC("ReplaceBoxa");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, boxaaGetCount(boxaa));
    Boxa *boxa = ll_check_Boxa(_fun, L, 3);
    return ll_push_boolean(_fun, L, boxa && 0 == boxaaReplaceBoxa(boxaa, idx, boxa));
}

/**
 * \brief Write a Boxaa* (%boxaa) to a file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaaWrite(filename, boxaa));
}

/**
 * \brief Write a Boxaa* (%boxaa) to memory (%data).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 *
 * Leptonica's Notes:
 *      (1) Serializes a boxaa in memory and puts the result in a buffer.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (boxaaWriteMem(&data, &size, boxaa))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    LEPT_FREE(data);
    return 1;
}

/**
 * \brief Write a Boxaa* (%boxaa) to a stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Box* (box).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Boxaa *boxaa = ll_check_Boxaa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == boxaaWriteStream(stream->f, boxaa));
}

/**
 * \brief Check Lua stack at index %arg for udata of class Boxaa.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxaa* contained in the user data
 */
Boxaa *
ll_check_Boxaa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Boxaa>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Boxaa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Boxaa* contained in the user data
 */
Boxaa *
ll_opt_Boxaa(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Boxaa(_fun, L, arg);
}

/**
 * \brief Push Boxaa* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param boxaa pointer to the BOXAA
 * \return 1 Boxaa* on the Lua stack
 */
int
ll_push_Boxaa(const char *_fun, lua_State *L, Boxaa *boxaa)
{
    if (!boxaa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, boxaa);
}
/**
 * \brief Create and push a new Boxaa*.
 * \param L pointer to the lua_State
 * \return 1 Boxaa* on the Lua stack
 */
int
ll_new_Boxaa(lua_State *L)
{
    FUNC("ll_new_Boxaa");
    Boxaa *boxaa = nullptr;

    if (lua_isuserdata(L, 1)) {
        Boxaa *boxaas = ll_opt_Boxaa(_fun, L, 1);
        if (boxaas) {
            DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
                LL_BOXAA, reinterpret_cast<void *>(boxaas));
            boxaa = boxaaCopy(boxaas, L_COPY);
        } else {
            luaL_Stream *stream = ll_check_stream(_fun, L, 2);
            DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
                LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
            boxaa = boxaaReadStream(stream->f);
        }
    }
    if (!boxaa && lua_isinteger(L, 1)) {
        l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        boxaa = boxaaCreate(n);
    }

    if (!boxaa && lua_isstring(L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        boxaa = boxaaRead(filename);
    }

    if (!boxaa && lua_isstring(L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        boxaa = boxaaReadMem(data, size);
    }

    if (!boxaa) {
        l_int32 n = 1;
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        boxaa = boxaaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(boxaa));
    return ll_push_Boxaa(_fun, L, boxaa);
}
/**
 * \brief Register the BOX methods and functions in the LL_BOX meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
luaopen_Boxaa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Boxaa},
        {"__len",                   GetCount},
        {"__tostring",              toString},
        {"AddBoxa",                 AddBoxa},
        {"Copy",                    Copy},
        {"Create",                  Create},
        {"Destroy",                 Destroy},
        {"ExtendArray",             ExtendArray},
        {"ExtendArrayToSize",       ExtendArrayToSize},
        {"FlattenAligned",          FlattenAligned},
        {"FlattenToBoxa",           FlattenToBoxa},
        {"GetBox",                  GetBox},
        {"GetBoxCount",             GetBoxCount},
        {"GetBoxa",                 GetBoxa},
        {"GetCount",                GetCount},
        {"InsertBoxa",              InsertBoxa},
        {"Join",                    Join},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RemoveBoxa",              RemoveBoxa},
        {"ReplaceBoxa",             ReplaceBoxa},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };

    FUNC("luaopen_" TNAME);

    ll_global_cfunct(_fun, L, TNAME, ll_new_Boxaa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
