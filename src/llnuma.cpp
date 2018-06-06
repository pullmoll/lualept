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
 * \file llnuma.cpp
 * \class Numa
 *
 * An array of numbers (l_float32).
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_NUMA

/** Define a function's name (_fun) with prefix Numa */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Numa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the numa.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Numa **pna = ll_check_udata<Numa>(_fun, L, 1, TNAME);
    Numa *na = *pna;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d, %s = %d\n", _fun,
        TNAME,
        "pna", reinterpret_cast<void *>(pna),
        "na", reinterpret_cast<void *>(na),
        "count", numaGetCount(na),
        "refcount", numaGetRefcount(na));
    numaDestroy(&na);
    *pna = nullptr;
    return 0;
}

/**
 * \brief Get the number of numbers stored in the Numa* (%na).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Numa* na = ll_check_Numa(_fun, L, 1);
    ll_push_l_int32(_fun, L, numaGetCount(na));
    return 1;
}

/**
 * \brief Replace one number to the Numa* at the given index.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na)
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_float32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplaceNumber(lua_State *L)
{
    LL_FUNC("ReplaceNumber");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 val = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == numaReplaceNumber(na, idx, val));
}

/**
 * \brief Printable string for a Numa*.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Numa *da = ll_check_Numa(_fun, L, 1);
    luaL_Buffer B;
    l_int32 i;
    l_float32 val;

    luaL_buffinit(L, &B);
    if (!da) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), TNAME ": %p", reinterpret_cast<void *>(da));
        luaL_addstring(&B, str);
        for (i = 0; i < numaGetCount(da); i++) {
            numaGetFValue(da, i, &val);
            snprintf(str, sizeof(str), "\n    %d = %.8g",
                     i+1, static_cast<double>(val));
            luaL_addstring(&B, str);
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Add one number (%val) to the Numa* (%na).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a l_float32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddNumber(lua_State *L)
{
    LL_FUNC("AddNumber");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_float32 val = ll_check_l_float32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == numaAddNumber(na, val));
}

/**
 * \brief Clone a Numa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (nas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
Clone(lua_State *L)
{
    LL_FUNC("Clone");
    Numa *nas = ll_check_Numa(_fun, L, 1);
    Numa *na = numaClone(nas);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Convert the Numa* (%na) to a string array (%sa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a string describing the number type (type).
 * Arg #3 is expected to be a l_int32 (size1).
 * Arg #4 is expected to be a l_int32 (size2) for 'float'.
 * Arg #5 is expected to be a boolean (addzeroes) for 'integer'.
 *
 * Leptonica's Notes:
 *      (1) For integer conversion, size2 is ignored.
 *          For float conversion, addzeroes is ignored.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sarray* (%sa) on the Lua stack
 */
static int
ConvertToSarray(lua_State *L)
{
    LL_FUNC("ConvertToSarray");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 type = ll_check_number_value(_fun, L, 2, L_FLOAT_VALUE);
    l_int32 size1 = ll_opt_l_int32(_fun, L, 3, 0);
    l_int32 size2 = L_FLOAT_VALUE == type ? ll_opt_l_int32(_fun, L, 4, 0) : 0;
    l_int32 addzeroes = ll_opt_boolean(_fun, L, 5, FALSE);
    Sarray *sa = numaConvertToSarray(na, size1, size2, addzeroes, type);
    ll_pack_Sarray(_fun, L, sa);
    sarrayDestroy(&sa);
    return 1;
}

/**
 * \brief Copy a Numa* (%nas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (nas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Numa *nas = ll_check_Numa(_fun, L, 1);
    Numa *na = numaCopy(nas);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Copy the parameters of the Numa* (%nas) to the Numa* (%nad).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (nad).
 * Arg #2 is expected to be another Numa* (nas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
CopyParameters(lua_State *L)
{
    LL_FUNC("CopyParameters");
    Numa *nad = ll_check_Numa(_fun, L, 1);
    Numa *nas = ll_check_Numa(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == numaCopyParameters(nad, nas));
}

/**
 * \brief Create a new Numa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Numa* na = numaCreate(n);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Set the number of numbers stored in the Numa* (%na) to zero.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa*.
 *
 * Leptonica's Notes:
 *      (1) This does not change the allocation of the array.
 *          It just clears the number of stored numbers, so that
 *          the array appears to be empty.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Empty(lua_State *L)
{
    LL_FUNC("Empty");
    Numa *na = ll_check_Numa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == numaEmpty(na));
}

/**
 * \brief Create a new Numa* from an array table of numbers.
 * <pre>
 * Arg #1 is expected to be a table (tbl).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
FromArray(lua_State *L)
{
    LL_FUNC("FromArray");
    l_int32 i, n;
    l_float32 *tbl = ll_unpack_Farray(_fun, L, 1, &n);
    Numa* na = numaCreate(n);
    if (!na)
        return ll_push_nil(L);
    for (i = 0; i < n; i++)
        numaAddNumber(na, tbl[i]);
    ll_free(tbl);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Get the Numa* (%na) as a table of lua_Number.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa*.
 *
 * Leptonica's Notes:
 *      (1) If copyflag == L_COPY, it makes a copy which the caller
 *          is responsible for freeing.  Otherwise, it operates
 *          directly on the bare array of the numa.
 *      (2) Very important: for L_NOCOPY, any writes to the array
 *          will be in the numa.  Do not write beyond the size of
 *          the count field, because it will not be accessible
 *          from the numa!  If necessary, be sure to set the count
 *          field to a larger number (such as the alloc size)
 *          BEFORE calling this function.  Creating with numaMakeConstant()
 *          is another way to insure full initialization.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetFArray(lua_State *L)
{
    LL_FUNC("GetFArray");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_float32 *farray = numaGetFArray(na, L_COPY);
    l_int32 n = numaGetCount(na);
    int res = ll_pack_Farray(_fun, L, farray, n);
    ll_free(farray);
    return res;
}

/**
 * \brief Get the number from the Numa* (%na) at index (%idx) as l_float32.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) Caller may need to check the function return value to
 *          decide if a 0.0 in the returned ival is valid.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetFValue(lua_State *L)
{
    LL_FUNC("GetFValue");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 val;
    if (numaGetFValue(na, idx, &val))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, val);
    return 1;
}

/**
 * \brief Get the Numa* (%na) as an array of lua_Integer.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa*.
 *
 * Leptonica's Notes:
 *      (1) A copy of the array is always made, because we need to
 *          generate an integer array from the bare float array.
 *          The caller is responsible for freeing the array.
 *      (2) The array size is determined by the number of stored numbers,
 *          not by the size of the allocated array in the Numa.
 *      (3) This function is provided to simplify calculations
 *          using the bare internal array, rather than continually
 *          calling accessors on the numa.  It is typically used
 *          on an array of size 256.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
GetIArray(lua_State *L)
{
    LL_FUNC("GetIArray");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 *iarray = numaGetIArray(na);
    l_int32 n = numaGetCount(na);
    int res = ll_pack_Iarray(_fun, L, iarray, n);
    ll_free(iarray);
    return res;
}

/**
 * \brief Get the number from the Numa* (%na) at index (%idx) as l_int32.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) Caller may need to check the function return value to
 *          decide if a 0 in the returned ival is valid.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetIValue(lua_State *L)
{
    LL_FUNC("GetIValue");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_int32 val;
    if (numaGetIValue(na, idx, &val))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, val);
    return 1;
}

/**
 * \brief Get the parameters of the Numa* (%na).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 numbers (startx, deltax) on the Lua stack
 */
static int
GetParameters(lua_State *L)
{
    LL_FUNC("GetParameters");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_float32 startx = 0;
    l_float32 deltax = 0;
    if (numaGetParameters(na, &startx, &deltax))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, startx);
    ll_push_l_float32(_fun, L, deltax);
    return 2;
}

/**
 * \brief Insert one number (%val) to the Numa* (%na) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_float32 (val).
 *
 * Leptonica's Notes:
 *      (1) This shifts na[i] --> na[i + 1] for all i >= index,
 *          and then inserts val as na[index].
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
InsertNumber(lua_State *L)
{
    LL_FUNC("InsertNumber");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 val = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == numaInsertNumber(na, idx, val));
}

/**
 * \brief Read a Numa* (%na) from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Numa *na = numaRead(filename);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Read a Numa* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Numa *na = numaReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Read a Numa* (%na) from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Numa *na = numaReadStream(stream->f);
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Remove one number to the Numa* (%na) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa*.
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) This shifts na[i] --> na[i - 1] for all i > index.
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
RemoveNumber(lua_State *L)
{
    LL_FUNC("RemoveNumber");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    return ll_push_boolean(_fun, L, 0 == numaRemoveNumber(na, idx));
}

/**
 * \brief Set the number of numbers stored in the Numa* (%na), i.e. resize Numa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a l_int32 (n).
 *
 * Leptonica's Notes:
 *      (1) If newcount <= na->nalloc, this resets na->n.
 *          Using newcount = 0 is equivalent to numaEmpty().
 *      (2) If newcount > na->nalloc, this causes a realloc
 *          to a size na->nalloc = newcount.
 *      (3) All the previously unused values in na are set to 0.0.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetCount(lua_State *L)
{
    LL_FUNC("SetCount");
    Numa* na = ll_check_Numa(_fun, L, 1);
    l_int32 n = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == numaSetCount(na, n));
}

/**
 * \brief Set the parameters (%startx, %deltax) of the Numa* (%na).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a l_float32 (startx).
 * Arg #3 is expected to be a l_float32 (deltax).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetParameters(lua_State *L)
{
    LL_FUNC("SetParameters");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_float32 startx = ll_check_l_float32(_fun, L, 2);
    l_float32 deltax = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == numaSetParameters(na, startx, deltax));
}

/**
 * \brief Set the value (%val) for the Numa* (%na) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* (na).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_float32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetValue(lua_State *L)
{
    LL_FUNC("SetValue");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 val = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == numaSetValue(na, idx, val));
}

/**
 * \brief Add a difference (%diff) to the value for the Numa* (%na) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa*.
 * Arg #2 is expected to be a l_int32 for the.
 * index Arg #3 is expected to be a l_float32
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ShiftValue(lua_State *L)
{
    LL_FUNC("ShiftValue");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, numaGetCount(na));
    l_float32 diff = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == numaShiftValue(na, idx, diff));
}

/**
 * \brief Write the Numa* to an external file.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa*.
 * Arg #2 is expected to be string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Numa *na = ll_check_Numa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == numaWrite(filename, na));
}

/**
 * \brief Write the Numa* (%na) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa* user data.
 *
 * Leptonica's Notes:
 *      (1) Serializes a numa in memory and puts the result in a buffer.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Numa *na = ll_check_Numa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (numaWriteMem(&data, &size, na))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write the Numa* (%na) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Numa*.
 * Arg #2 is expected to be string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Numa *na = ll_check_Numa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == numaWriteStream(stream->f, na));
}

/**
 * \brief Check Lua stack at index %arg for udata of class Numa*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Numa* contained in the user data
 */
Numa *
ll_check_Numa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Numa>(_fun, L, arg, TNAME);
}

/**
 * \brief Check Lua stack at index %arg for udata of class Numa* and take it.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Numa* contained in the user data
 */
Numa *
ll_take_Numa(const char *_fun, lua_State *L, int arg)
{
    Numa **pna = ll_check_udata<Numa>(_fun, L, arg, TNAME);
    Numa *na = *pna;
    DBG(LOG_TAKE, "%s: '%s' %s = %p, %s = %p\n", _fun,
        TNAME,
        "pna", reinterpret_cast<void *>(pna),
        "na", reinterpret_cast<void *>(na));
    *pna = nullptr;
    return na;
}

/**
 * \brief Take a Numa* from a global variable %name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param name of the global variable
 * \return pointer to the Amap* contained in the user data
 */
Numa *
ll_get_global_Numa(const char *_fun, lua_State *L, const char *name)
{
    if (LUA_TUSERDATA != lua_getglobal(L, name))
        return nullptr;
    return ll_take_Numa(_fun, L, -1);
}

/**
 * \brief Optionally expect a Numa* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Numa* contained in the user data
 */
Numa *
ll_opt_Numa(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Numa(_fun, L, arg);
}

/**
 * \brief Push NUMA user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param na pointer to the NUMA
 * \return 1 Numa* on the Lua stack
 */
int
ll_push_Numa(const char *_fun, lua_State *L, Numa *na)
{
    if (!na)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, na);
}

/**
 * \brief Create and push NUMA user data to the Lua stack.
 * \param L pointer to the lua_State
 * \return 1 Numa* on the Lua stack
 */
int
ll_new_Numa(lua_State *L)
{
    FUNC("ll_new_Numa");
    Numa *na = nullptr;
    Numa *das = nullptr;
    const char *filename = nullptr;
    luaL_Stream* stream = nullptr;
    const l_uint8 *data = nullptr;
    size_t size = 0;
    l_int32 n = 1;

    if (ll_isudata(_fun, L, 1, LL_NUMA)) {
        das = ll_opt_Numa(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            TNAME, reinterpret_cast<void *>(das));
        na = numaCopy(das);
    }

    if (!na && ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        na = numaReadStream(stream->f);
    }

    if (!na && ll_isinteger(_fun, L, 1)) {
        n = ll_opt_l_int32(_fun, L, 1, n);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        na = numaCreate(n);
    }

    if (!na && ll_isstring(_fun, L, 1)) {
        filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        na = numaRead(filename);
    }

    if (!na && ll_isstring(_fun, L, 1)) {
        data = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %llu\n", _fun,
            "data", reinterpret_cast<const void *>(data),
            "size", static_cast<l_uint64>(size));
        na = numaReadMem(data, size);
    }

    if (!na) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        na = numaCreate(n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(na));
    return ll_push_Numa(_fun, L, na);
}

/**
 * \brief Register the Numa methods and functions in the Numa meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_open_Numa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_Numa},
        {"__len",               GetCount},
        {"__newitem",           ReplaceNumber},
        {"__tostring",          toString},
        {"AddNumber",           AddNumber},
        {"Clone",               Clone},
        {"ConvertToSarray",     ConvertToSarray},
        {"Copy",                Copy},
        {"CopyParameters",      CopyParameters},
        {"Create",              Create},
        {"Empty",               Empty},
        {"FromArray",           FromArray},
        {"GetFArray",           GetFArray},
        {"GetFValue",           GetFValue},
        {"GetIArray",           GetIArray},
        {"GetIValue",           GetIValue},
        {"GetParameters",       GetParameters},
        {"InsertNumber",        InsertNumber},
        {"Read",                Read},
        {"ReadMem",             ReadMem},
        {"ReadStream",          ReadStream},
        {"RemoveNumber",        RemoveNumber},
        {"ReplaceNumber",       ReplaceNumber},
        {"SetCount",            SetCount},
        {"SetParameters",       SetParameters},
        {"SetValue",            SetValue},
        {"ShiftValue",          ShiftValue},
        {"Write",               Write},
        {"WriteMem",            WriteMem},
        {"WriteStream",         WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Numa);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
