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
 * \file lldna.cpp
 * \class Dna
 *
 * An array of double (l_float64) numbers.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_DNA

/** Define a function's name (_fun) with prefix Dna */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Dna* (%da).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the l_dna.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Dna *da = ll_take_udata<Dna>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d, %s = %d\n", _fun,
        TNAME,
        "da", reinterpret_cast<void *>(da),
        "count", l_dnaGetCount(da),
        "refcount", l_dnaGetRefcount(da));
    l_dnaDestroy(&da);
    return 0;
}

/**
 * \brief Get the number of numbers stored in the Dna* (%da).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Dna *da = ll_check_Dna(_fun, L, 1);
    ll_push_l_int32(_fun, L, l_dnaGetCount(da));
    return 1;
}

/**
 * \brief Replace one number to the Dna* (%da) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a lua_Number to use to replace.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ReplaceNumber(lua_State *L)
{
    LL_FUNC("ReplaceNumber");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 3, &isnumber);
    if (!isnumber)
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, 0 == l_dnaReplaceNumber(da, idx, val));
}

/**
 * \brief Printable string for a Dna* (%da).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Dna *da = ll_check_Dna(_fun, L, 1);
    luaL_Buffer B;
    l_float64 val;

    luaL_buffinit(L, &B);
    if (!da) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(da));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        for (l_int32 i = 0; i < l_dnaGetCount(da); i++) {
            l_dnaGetDValue(da, i, &val);
            snprintf(str, LL_STRBUFF,
                     "\n    %d = %.15g", i+1, val);
            luaL_addstring(&B, str);
        }
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Add one number to the Dna* (%da).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a lua_Number to add to the array.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddNumber(lua_State *L)
{
    LL_FUNC("AddNumber");
    Dna *da = ll_check_Dna(_fun, L, 1);
    int isnumber = 0;
    lua_Number val = lua_tonumberx(L, 2, &isnumber);
    if (!isnumber)
        return ll_push_nil(L);
    return ll_push_boolean(_fun, L, 0 == l_dnaAddNumber(da, val));
}

/**
 * \brief Clone a Dna* (%das).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (das).
 * </pre>
 * \param L Lua state.
 * \return 1 Dna* (%da) on the Lua stack.
 */
static int
Clone(lua_State *L)
{
    LL_FUNC("Clone");
    Dna *das = ll_check_Dna(_fun, L, 1);
    Dna *da = l_dnaClone(das);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Copy a Dna* (%das).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (das).
 *
 * Leptonica's Notes:
 *      (1) This removes unused ptrs above da->n.
 * </pre>
 * \param L Lua state.
 * \return 1 Dna* (%da) on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Dna *das = ll_check_Dna(_fun, L, 1);
    Dna *da = l_dnaCopy(das);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Copy the parameters of the Dna* (%das) to another Dna* (%dad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* user data (destination).
 * Arg #2 is expected to be another Dna* user data (source).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
CopyParameters(lua_State *L)
{
    LL_FUNC("CopyParameters");
    Dna *dad = ll_check_Dna(_fun, L, 1);
    Dna *das = ll_check_Dna(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == l_dnaCopyParameters(dad, das));
}

/**
 * \brief Create a new Dna* (%da).
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L Lua state.
 * \return 1 Dna* (%da) on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Dna *da = l_dnaCreate(n);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Set the number of numbers stored in the Dna* (%da) to zero.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 *
 * Leptonica's Notes:
 *      (1) This does not change the allocation of the array.
 *          It just clears the number of stored numbers, so that
 *          the array appears to be empty.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Empty(lua_State *L)
{
    LL_FUNC("Empty");
    Dna *da = ll_check_Dna(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == l_dnaEmpty(da));
}

/**
 * \brief Create a new Dna* (%da) from an array table of numbers.
 * <pre>
 * Arg #1 is expected to be a table (tbl).
 * </pre>
 * \param L Lua state.
 * \return 1 Dna* (%da) on the Lua stack.
 */
static int
FromArray(lua_State *L)
{
    LL_FUNC("FromArray");
    l_int32 i, n;
    l_float64 *tbl = ll_unpack_Darray(_fun, L, 1, &n);
    Dna* da = l_dnaCreate(n);
    if (!da)
        return ll_push_nil(L);
    for (i = 0; i < n; i++)
        l_dnaAddNumber(da, tbl[i]);
    ll_free(tbl);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Get the Dna* (%da) as table of lua_Number.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 *
 * Leptonica's Notes:
 *      (1) If copyflag == L_COPY, it makes a copy which the caller
 *          is responsible for freeing.  Otherwise, it operates
 *          directly on the bare array of the l_dna.
 *      (2) Very important: for L_NOCOPY, any writes to the array
 *          will be in the l_dna.  Do not write beyond the size of
 *          the count field, because it will not be accessible
 *          from the l_dna!  If necessary, be sure to set the count
 *          field to a larger number (such as the alloc size)
 *          BEFORE calling this function.  Creating with l_dnaMakeConstant()
 *          is another way to insure full initialization.
 * </pre>
 * \param L Lua state.
 * \return 1 table array on the Lua stack.
 */
static int
GetDArray(lua_State *L)
{
    LL_FUNC("GetDArray");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_float64 *darray = l_dnaGetDArray(da, L_COPY);
    l_int32 n = l_dnaGetCount(da);
    int res = ll_pack_Darray(_fun, L, darray, n);
    ll_free(darray);
    return res;
}

/**
 * \brief Get the l_float64 from the Dna* (%da) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) Caller may need to check the function return value to
 *          decide if a 0.0 in the returned ival is valid.
 * </pre>
 * \param L Lua state.
 * \return 1 number on the Lua stack.
 */
static int
GetDValue(lua_State *L)
{
    LL_FUNC("GetDValue");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    l_float64 val = 0;
    if (l_dnaGetDValue(da, idx, &val))
        return ll_push_nil(L);
    ll_push_l_float64(_fun, L, val);
    return 1;
}

/**
 * \brief Get the Dna* (%da) as a table of lua_Integer.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 *
 * Leptonica's Notes:
 *      (1) A copy of the array is made, because we need to
 *          generate an integer array from the bare double array.
 *          The caller is responsible for freeing the array.
 *      (2) The array size is determined by the number of stored numbers,
 *          not by the size of the allocated array in the l_dna.
 *      (3) This function is provided to simplify calculations
 *          using the bare internal array, rather than continually
 *          calling accessors on the l_dna.  It is typically used
 *          on an array of size 256.
 * </pre>
 * \param L Lua state.
 * \return 1 table array on the Lua stack.
 */
static int
GetIArray(lua_State *L)
{
    LL_FUNC("GetIArray");
    Dna *dna = ll_check_Dna(_fun, L, 1);
    l_int32 *iarray = l_dnaGetIArray(dna);
    l_int32 size = l_dnaGetCount(dna);
    int res = ll_pack_Iarray(_fun, L, iarray, size);
    ll_free(iarray);
    return res;
}

/**
 * \brief Get one integer from the Dna* (%da) at index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a index (idx).
 *
 * Leptonica's Notes:
 *      (1) Caller may need to check the function return value to
 *          decide if a 0 in the returned ival is valid.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetIValue(lua_State *L)
{
    LL_FUNC("GetIValue");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    l_int32 val;
    if (l_dnaGetIValue(da, idx, &val))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, val);
    return 1;
}

/**
 * \brief Get the parameters of the Dna*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * </pre>
 * \param L Lua state.
 * \return 2 numbers on the Lua stack (%startx, %deltax).
 */
static int
GetParameters(lua_State *L)
{
    LL_FUNC("GetParameters");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_float64 startx = 0;
    l_float64 deltax = 0;
    if (l_dnaGetParameters(da, &startx, &deltax))
        return ll_push_nil(L);
    ll_push_l_float64(_fun, L, startx);
    ll_push_l_float64(_fun, L, deltax);
    return 2;
}

/**
 * \brief Insert one number to the Dna* (%da) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a lua_Number to insert into the array.
 *
 * Leptonica's Notes:
 *      (1) This shifts da[i] --> da[i + 1] for all i >= index,
 *          and then inserts val as da[index].
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
InsertNumber(lua_State *L)
{
    LL_FUNC("InsertNumber");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    l_float64 val = ll_check_l_float64(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == l_dnaInsertNumber(da, idx, val));
}

/**
 * \brief Read a Dna* (%da) from a file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Dna *da = l_dnaRead(filename);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Read a Dna* (%da) from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 *
 * Leptonica's Notes:
 *      (1) fscanf takes %lf to read a double; fprintf takes %f to write it.
 * </pre>
 * \param L Lua state.
 * \return 1 Box* on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Dna *da = l_dnaReadStream(stream->f);
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Remove one number to the Dna* (%da) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) This shifts da[i] --> da[i - 1] for all i > index.
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
RemoveNumber(lua_State *L)
{
    LL_FUNC("RemoveNumber");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    return ll_push_boolean(_fun, L, 0 == l_dnaRemoveNumber(da, idx));
}

/**
 * \brief Set the number of numbers stored in the Dna* (%da), i.e. resize Dna*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a l_int32 (n).
 *
 * Leptonica's Notes:
 *      (1) If newcount <= da->nalloc, this resets da->n.
 *          Using newcount = 0 is equivalent to l_dnaEmpty().
 *      (2) If newcount > da->nalloc, this causes a realloc
 *          to a size da->nalloc = newcount.
 *      (3) All the previously unused values in da are set to 0.0.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetCount(lua_State *L)
{
    LL_FUNC("SetCount");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == l_dnaSetCount(da, n));
}

/**
 * \brief Set the parameters (%startx, %deltax) of the Dna* (%da).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a l_float64 (startx).
 * Arg #3 is expected to be a l_float64 (deltax).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetParameters(lua_State *L)
{
    LL_FUNC("SetParameters");
    Dna *da = ll_check_Dna(_fun, L, 1);
    lua_Number startx = ll_check_l_float64(_fun, L, 2);
    lua_Number deltax = ll_check_l_float64(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == l_dnaSetParameters(da, startx, deltax));
}

/**
 * \brief Set the value for the Dna* (%da) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a lua_Number to set in the array.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetValue(lua_State *L)
{
    LL_FUNC("SetValue");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    l_float64 val = ll_check_l_float64(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == l_dnaSetValue(da, idx, val));
}

/**
 * \brief Add a difference (%diff) to the value for the Dna* (%da) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_float64 (diff).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
ShiftValue(lua_State *L)
{
    LL_FUNC("ShiftValue");
    Dna *da = ll_check_Dna(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, l_dnaGetCount(da));
    l_float64 diff = ll_check_l_float64(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == l_dnaShiftValue(da, idx, diff));
}

/**
 * \brief Write a Dna* (%da) to a file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Dna *da = ll_check_Dna(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == l_dnaWrite(filename, da));
}

/**
 * \brief Write a Dna* (%da) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dna* (da).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Dna *da = ll_check_Dna(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == l_dnaWriteStream(stream->f, da));
}

/**
 * \brief Check Lua stack at index %arg for user data of class Dna*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dna* contained in the user data.
 */
Dna *
ll_check_Dna(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Dna>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Dna* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dna* contained in the user data.
 */
Dna *
ll_opt_Dna(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Dna(_fun, L, arg);
}

/**
 * \brief Push DNA user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param da pointer to the L_DNA
 * \return 1 DNA* on the Lua stack.
 */
int
ll_push_Dna(const char *_fun, lua_State *L, Dna *da)
{
    if (!da)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, da);
}

/**
 * \brief Create and push a new DNA*.
 * \param L Lua state.
 * \return 1 DNA* on the Lua stack.
 */
int
ll_new_Dna(lua_State *L)
{
    FUNC("ll_new_Dna");
    Dna *da = nullptr;
    l_int32 n = 1;

    if (ll_isudata(_fun, L, 1, LL_DNA)) {
        Dna *das = ll_opt_Dna(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            TNAME, reinterpret_cast<void *>(das));
        da = l_dnaCopy(das);
    }

    if (ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        luaL_Stream* stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        da = l_dnaReadStream(stream->f);
    }

    if (ll_isinteger(_fun, L, 1)) {
        l_int32 n = ll_opt_l_int32(_fun, L, 1, n);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        da = l_dnaCreate(n);
    }

    if (!da && ll_isstring(_fun, L, 1)) {
        const char *filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        da = l_dnaRead(filename);
    }

    if (!da) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        da = l_dnaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(da));
    return ll_push_Dna(_fun, L, da);
}

/**
 * \brief Register the L_DNA methods and functions in the LL_L_DNA meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Dna(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Dna},
        {"__len",                   GetCount},
        {"__newitem",               ReplaceNumber},
        {"__tostring",              toString},
        {"AddNumber",               AddNumber},
        {"Clone",                   Clone},
        {"Copy",                    Copy},
        {"CopyParameters",          CopyParameters},
        {"Create",                  Create},
        {"Empty",                   Empty},
        {"FromArray",               FromArray},
        {"GetDArray",               GetDArray},
        {"GetFValue",               GetDValue},
        {"GetIArray",               GetIArray},
        {"GetIValue",               GetIValue},
        {"GetParameters",           GetParameters},
        {"InsertNumber",            InsertNumber},
        {"Read",                    Read},
        {"ReadStream",              ReadStream},
        {"RemoveNumber",            RemoveNumber},
        {"ReplaceNumber",           ReplaceNumber},
        {"SetCount",                SetCount},
        {"SetParameters",           SetParameters},
        {"SetValue",                SetValue},
        {"ShiftValue",              ShiftValue},
        {"Write",                   Write},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Dna);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
