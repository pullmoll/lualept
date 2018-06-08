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
 * \file llpixa.cpp
 * \class Pixa
 *
 * An array of Pix.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PIXA

/** Define a function's name (_fun) with prefix Pixa */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Pixa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the pixa.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L Lua state
 * \return 0 nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Pixa *pixa = ll_take_udata<Pixa>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "pixa", reinterpret_cast<void *>(pixa),
        "count", pixaGetCount(pixa));
    pixaDestroy(&pixa);
    return 0;
}

/**
 * \brief Get count for a Pixa* (%pixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * </pre>
 * \param L Lua state
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    ll_push_l_int32(_fun, L, pixaGetCount(pixa));
    return 1;
}

/**
 * \brief Add a Pix* (%pix) to a Pixa* (%pixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a Pix*.
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
AddPix(lua_State *L)
{
    LL_FUNC("AddPix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == pixaAddPix(pixa, pix, flag));
}

/**
 * \brief Clear the Pixa* (%pixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 *
 * Leptonica's Notes:
 *      (1) This destroys all pix in the pixa, as well as
 *          all boxes in the boxa.  The ptrs in the pix ptr array
 *          are all null'd.  The number of allocated pix, n, is set to 0.
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    LL_FUNC("Clear");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixaClear(pixa));
}

/**
 * \brief Copy a Pixa* (%pixas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is an optional string defining the storage flags (copy, clone, copy_clone).
 * </pre>
 * \param L Lua state
 * \return 1 Pixa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Pixa *pixas = ll_check_Pixa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Pixa *pixa = pixaCopy(pixas, copyflag);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Create a new Pixa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 *
 * Leptonica's Notes:
 *      (1) This creates an empty boxa.
 * </pre>
 * \param L Lua state
 * \return 1 Pixa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Pixa *pixa = pixaCreate(n);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief CreateFromPix() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (n).
 * Arg #3 is expected to be a l_int32 (cellw).
 * Arg #4 is expected to be a l_int32 (cellh).
 *
 * Leptonica's Notes:
 *      (1) For bpp = 1, we truncate each retrieved pix to the ON
 *          pixels, which we assume for now start at (0,0)
 * </pre>
 * \param L Lua state
 * \return 1 on the Lua stack
 */
static int
CreateFromPix(lua_State *L)
{
    LL_FUNC("CreateFromPix");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
    l_int32 cellw = ll_opt_l_int32(_fun, L, 3, pixGetWidth(pixs));
    l_int32 cellh = ll_opt_l_int32(_fun, L, 4, pixGetHeight(pixs));
    Pixa *pixa = pixaCreateFromPix(pixs, n, cellw, cellh);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief CreateFromPixacomp() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a PixaComp* (pixac).
 * Arg #2 is expected to be a l_int32 (accesstype).
 *
 * Leptonica's Notes:
 *      (1) Because the pixa has no notion of offset, the offset must
 *          be set to 0 before the conversion, so that pixacompGetPix()
 *          fetches all the pixcomps.  It is reset at the end.
 * </pre>
 * \param L Lua state
 * \return 1 on the Lua stack
 */
static int
CreateFromPixacomp(lua_State *L)
{
    LL_FUNC("CreateFromPixacomp");
    PixaComp *pixac = ll_check_PixaComp(_fun, L, 1);
    l_int32 accesstype = ll_check_access_storage(_fun, L, 2, L_COPY);
    Pixa *pixa = pixaCreateFromPixacomp(pixac, accesstype);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Get pixel aligned statistics for Pixa* (%pixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 *
 * Leptonica's Notes:
 *      (1) Each pixel in the returned pix represents an average
 *          (or median, or mode) over the corresponding pixels in each
 *          pix in the pixa.
 *      (2) The %thresh parameter works with L_MODE_VAL only, and
 *          sets a minimum occupancy of the mode bin.
 *          If the occupancy of the mode bin is less than %thresh, the
 *          mode value is returned as 0.  To always return the actual
 *          mode value, set %thresh = 0.  See pixGetRowStats().
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
GetAlignedStats(lua_State *L)
{
    LL_FUNC("GetAlignedStats");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 type = ll_check_stats_type(_fun, L, 2, L_MEAN_ABSVAL);
    l_int32 nbins = ll_opt_l_int32(_fun, L, 3, 2);
    l_int32 thresh = ll_opt_l_int32(_fun, L, 4, 0);
    Pix *pix = pixaGetAlignedStats(pixa, type, nbins, thresh);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get box geometry for a Pix* from a Pixa* (%pixa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state
 * \return 4 integers on the Lua stack: x, y, w, h
 */
static int
GetBoxGeometry(lua_State *L)
{
    LL_FUNC("GetBoxGeometry");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    l_int32 x, y, w, h;
    if (pixaGetBoxGeometry(pixa, idx, &x, &y, &w, &h))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, x);
    ll_push_l_int32(_fun, L, y);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 4;
}

/**
 * \brief Get box geometry for a Pix* from a Pixa* (%pixa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state
 * \return 4 integers on the Lua stack: x, y, w, h
 */
static int
GetPix(lua_State *L)
{
    LL_FUNC("GetPix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    Pix *pix = pixaGetPix(pixa, idx, copyflag);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Insert the Pix* (%pixs) in a Pixa* (%pixa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix* (%pixs).
 * Arg #4 is an optional Box* (%boxs).
 *
 * Leptonica's Notes:
 *      (1) This shifts pixa[i] --> pixa[i + 1] for all i >= index,
 *          and then inserts at pixa[index].
 *      (2) To insert at the beginning of the array, set index = 0.
 *      (3) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 *      (4) To append a pix to a pixa, it's easier to use pixaAddPix().
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
InsertPix(lua_State *L)
{
    LL_FUNC("InsertPix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    Box *boxs = ll_opt_Box(_fun, L, 4);
    Pix *pix = pixClone(pixs);
    Box *box = boxs ? boxClone(boxs) : nullptr;
    lua_pushboolean(L, pix && 0 == pixaInsertPix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief Interleave two Pixa* (%pixa1, %pixa2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa1).
 * Arg #2 is expected to be another Pixa* (pixa2).
 * Arg #3 is an optional string defining the storage flags (copy, clone, copy_clone).
 *
 * Leptonica's Notes:
 *      (1) %copyflag determines if the pix are copied or cloned.
 *          The boxes, if they exist, are copied.
 *      (2) If the two pixa have different sizes, a warning is issued,
 *          and the number of pairs returned is the minimum size.
 * </pre>
 * \param L Lua state
 * \return 1 Pixa* on the Lua stack
 */
static int
Interleave(lua_State *L)
{
    LL_FUNC("Interleave");
    Pixa *pixa1 = ll_check_Pixa(_fun, L, 1);
    Pixa *pixa2 = ll_check_Pixa(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_CLONE);
    Pixa *pixa = pixaInterleave(pixa1, pixa2, copyflag);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Join Pixa* (%pixas) to Pixa* (%pixad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixad).
 * Arg #2 is expected to be another Pixa* (pixas).
 *
 * Leptonica's Notes:
 *      (1) This appends a clone of each indicated pix in pixas to pixad
 *      (2) istart < 0 is taken to mean 'read from the start' (istart = 0)
 *      (3) iend < 0 means 'read to the end'
 *      (4) If pixas is NULL or contains no pix, this is a no-op.
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    Pixa *pixad = ll_check_Pixa(_fun, L, 1);
    Pixa *pixas = ll_check_Pixa(_fun, L, 2);
    l_int32 istart = ll_opt_l_int32(_fun, L, 3, 1);
    l_int32 iend = ll_opt_l_int32(_fun, L, 3, pixaGetCount(pixas));
    return ll_push_boolean(_fun, L, 0 == pixaJoin(pixad, pixas, istart, iend));
}

/**
 * \brief Read a Pixa* from an external file.
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 *
 * Leptonica's Notes:
 *      (1) The pix are stored in the file as png.
 *          If the png library is not linked, this will fail.
 * </pre>
 * \param L Lua state
 * \return 1 Pixa* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Pixa *pixa = pixaRead(filename);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Read a Pixa* (%pixa) from a number of external files.
 * <pre>
 * Arg #1 is expected to be a string containing the directory (dirname).
 * Arg #2 is expected to be a string (substr).
 *
 * Leptonica's Notes:
 *      (1) %dirname is the full path for the directory.
 *      (2) %substr is the part of the file name (excluding
 *          the directory) that is to be matched.  All matching
 *          filenames are read into the Pixa.  If substr is NULL,
 *          all filenames are read into the Pixa.
 * </pre>
 * \param L Lua state
 * \return 1 Pixa* on the Lua stack
 */
static int
ReadFiles(lua_State *L)
{
    LL_FUNC("ReadFiles");
    const char *dirname = ll_check_string(_fun, L, 1);
    const char *substr = ll_check_string(_fun, L, 2);
    Pixa *pixa = pixaReadFiles(dirname, substr);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Read a Pixa* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L Lua state
 * \return 1 Pixa* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Pixa *pixa = pixaReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Read a Pixa* from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 *
 * Leptonica's Notes:
 *      (1) The pix are stored in the file as png.
 *          If the png library is not linked, this will fail.
 * </pre>
 * \param L Lua state
 * \return 1 Pixa* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pixa *pixa = pixaReadStream(stream->f);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Remove the Pix* from a Pixa* (%pixa) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) This shifts pixa[i] --> pixa[i - 1] for all i > index.
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 *      (3) The corresponding box is removed as well, if it exists.
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
RemovePix(lua_State *L)
{
    LL_FUNC("RemovePix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    return ll_push_boolean(_fun, L, 0 == pixaRemovePix(pixa, idx));
}

/**
 * \brief Remove the Pix* (%pix) from a Pixa* (%pixa) at index (%idx) and return it and its box.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa*.
 * Arg #2 is expected to be a l_int32 (%idx).
 *
 * Leptonica's Notes:
 *      (1) This shifts pixa[i] --> pixa[i - 1] for all i > index.
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 *      (3) The corresponding box is removed as well, if it exists.
 *      (4) The removed pix and box can either be retained or destroyed.
 * </pre>
 * \param L Lua state
 * \return 2 Pix* (pix) and one Box* (box) on the Lua stack
 */
static int
RemovePixAndSave(lua_State *L)
{
    LL_FUNC("RemovePixAndSave");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pix = nullptr;
    Box *box = nullptr;
    if (pixaRemovePixAndSave(pixa, idx, &pix, &box))
        return ll_push_nil(L);
    return ll_push_Pix(_fun, L, pix) + ll_push_Box(_fun, L, box);
}

/**
 * \brief Replace the Pix* in a Pixa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix* (pixs).
 * Arg #4 is an optional Box* (boxs).
 *
 * Leptonica's Notes:
 *      (1) In-place replacement of one pix.
 *      (2) The previous pix at that location is destroyed.
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
ReplacePix(lua_State *L)
{
    LL_FUNC("ReplacePix");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaGetCount(pixa));
    Pix *pixs = ll_check_Pix(_fun, L, 3);
    Box *boxs = ll_opt_Box(_fun, L, 4);
    Pix *pix = pixClone(pixs);
    Box *box = boxs ? boxClone(boxs) : nullptr;
    lua_pushboolean(L, pix && 0 == pixaReplacePix(pixa, idx, pix, box));
    return 1;
}

/**
 * \brief TemplatesFromComposites() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixac).
 * Arg #2 is expected to be a Numa* (na).
 *
 * </pre>
 * \param L Lua state
 * \return 1 on the Lua stack
 */
static int
TemplatesFromComposites(lua_State *L)
{
    LL_FUNC("TemplatesFromComposites");
    Pixa *pixac = ll_check_Pixa(_fun, L, 1);
    Numa *na = ll_check_Numa(_fun, L, 2);
    Pixa *pixa = jbTemplatesFromComposites(pixac, na);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Write the Pixa* (%pixa) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * Arg #2 is expected to be string containing the filename.
 *
 * Leptonica's Notes:
 *      (1) The pix are stored in the file as png.
 *          If the png library is not linked, this will fail.
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixaWrite(filename, pixa));
}

/**
 * \brief Write the Pixa* (%pixa) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 *
 * Leptonica's Notes:
 *      (1) Serializes a pixa in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixaWriteMem(&data, &size, pixa))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write the Pixa* to an Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 *
 * Leptonica's Notes:
 *      (1) The pix are stored in the file as png.
 *          If the png library is not linked, this will fail.
 * </pre>
 * \param L Lua state
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixaWriteStream(stream->f, pixa));
}

/**
 * \brief Display() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixa* (pixa).
 * Arg #2 is expected to be a l_int32 (w).
 * Arg #3 is expected to be a l_int32 (h).
 *
 * Leptonica's Notes:
 *      (1) This uses the boxes to place each pix in the rendered composite.
 *      (2) Set w = h = 0 to use the b.b. of the components to determine
 *          the size of the returned pix.
 *      (3) Uses the first pix in pixa to determine the depth.
 *      (4) The background is written "white".  On 1 bpp, each successive
 *          pix is "painted" (adding foreground), whereas for grayscale
 *          or color each successive pix is blitted with just the src.
 *      (5) If the pixa is empty, returns an empty 1 bpp pix.
 * </pre>
 * \param L Lua state
 * \return 1 on the Lua stack
 */
static int
Display(lua_State *L)
{
    LL_FUNC("Display");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 w = ll_opt_l_int32(_fun, L, 2, 0);
    l_int32 h = ll_opt_l_int32(_fun, L, 3, 0);
    Pix *pix = pixaDisplay(pixa, w, h);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Check Lua stack at index %arg for udata of class Pixa*.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PIXA contained in the user data
 */
Pixa *
ll_check_Pixa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Pixa>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Pixa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pixa* contained in the user data
 */
Pixa *
ll_opt_Pixa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Pixa(_fun, L, arg);
}

/**
 * \brief Push Pixa* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state
 * \param pixa pointer to the PIXA
 * \return 1 Pixa* on the Lua stack
 */
int
ll_push_Pixa(const char *_fun, lua_State *L, Pixa *pixa)
{
    if (!pixa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, pixa);
}

/**
 * \brief Create a new Pixa*.
 * \param L Lua state
 * \return 1 Pixa* on the Lua stack
 */
int
ll_new_Pixa(lua_State *L)
{
    FUNC("ll_new_Pixa");
    Pixa *pixa = nullptr;
    Pixa *pixas = nullptr;
    Pix *pixs = nullptr;
    luaL_Stream* stream = nullptr;
    l_int32 copyflag = L_COPY;
    l_int32 n = 1;
    l_int32 cellw = 32;
    l_int32 cellh = 32;

    if (ll_isudata(_fun, L, 1, LL_PIX)) {
        pixs = ll_opt_Pix(_fun, L, 1);
        n = ll_opt_l_int32(_fun, L, 2, 1);
        cellw = ll_opt_l_int32(_fun, L, 3, cellw);
        cellh = ll_opt_l_int32(_fun, L, 4, cellh);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pixs));
        pixa = pixaCreateFromPix(pixs, n, cellw, cellh);
    }

    if (!pixa && ll_isudata(_fun, L, 1, LL_PIXA)) {
        pixas = ll_opt_Pixa(_fun, L, 1);
        copyflag = ll_check_access_storage(_fun, L, 2, copyflag);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIXA, reinterpret_cast<void *>(pixs));
        pixa = pixaCopy(pixas, copyflag);
    }

    if (!pixa && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        pixa = pixaReadStream(stream->f);
    }

    if (!pixa && ll_isinteger(_fun, L, 1)) {
        n = ll_check_l_int32(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        pixa = pixaCreate(n);
    }

    if (!pixa && ll_isstring(_fun, L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
            DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
                "filename", filename);
        pixa = pixaRead(filename);
    }

    if (!pixa && ll_isstring(_fun, L, 1)) {
        size_t size = 0;
        const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        pixa = pixaReadMem(data, size);
    }

    if (!pixa) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        pixa = pixaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(pixa));
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table.
 * \param L Lua state
 * \return 1 table on the Lua stack
 */
int
ll_open_Pixa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Pixa},
        {"__len",                   GetCount},
        {"AddPix",                  AddPix},
        {"Clear",                   Clear},
        {"Copy",                    Copy},
        {"Create",                  Create},
        {"CreateFromPix",           CreateFromPix},
        {"CreateFromPixacomp",      CreateFromPixacomp},
        {"Destroy",                 Destroy},
        {"Display",                 Display},
        {"GetAlignedStats",         GetAlignedStats},
        {"GetBoxGeometry",          GetBoxGeometry},
        {"GetCount",                GetCount},
        {"GetPix",                  GetPix},
        {"InsertPix",               InsertPix},
        {"Interleave",              Interleave},
        {"Join",                    Join},
        {"Read",                    Read},
        {"ReadFiles",               ReadFiles},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RemovePix",               RemovePix},
        {"RemovePixAndSave",        RemovePixAndSave},
        {"ReplacePix",              ReplacePix},
        {"TakePix",                 RemovePixAndSave},  /* alias name */
        {"TemplatesFromComposites", TemplatesFromComposites},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Pixa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
