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
 * \file llsarray.cpp
 * \class Sarray
 *
 * An array of strings.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_SARRAY

/** Define a function's name (_fun) with prefix Sarray */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the sarray.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Sarray **psa = ll_check_udata<Sarray>(_fun, L, 1, TNAME);
    Sarray *sa = *psa;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p\n", _fun,
        TNAME,
        "psa", reinterpret_cast<void *>(psa),
        "sa", reinterpret_cast<void *>(sa));
    sarrayDestroy(&sa);
    *psa = nullptr;
    return 0;
}

/**
 * \brief Printable string for a Sarray*.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char str[256];
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!sa) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str), TNAME ": %p\n", reinterpret_cast<void *>(sa));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    nalloc = 0x%x, n = 0x%x, refcount = %d",
                 sa->nalloc, sa->n, sa->refcount);
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}


/**
 * \brief Create() brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sarray* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Sarray *result = sarrayCreate(n);
    return ll_push_Sarray(_fun, L, result);
}

/**
 * \brief AddString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a char* (string).
 * Arg #3 is expected to be a l_int32 (copyflag).
 *
 * Leptonica's Notes:
 *      (1) See usage comments at the top of this file.  L_INSERT is
 *          equivalent to L_NOCOPY.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
AddString(lua_State *L)
{
    LL_FUNC("AddString");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    const char *str = ll_check_string(_fun, L, 2);
    char *string = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(str));
    l_int32 copyflag = L_COPY;
    return ll_push_boolean(_fun, L, 0 == sarrayAddString(sa, string, copyflag));
}

/**
 * \brief Append() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a string (filename).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
Append(lua_State *L)
{
    LL_FUNC("Append");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_ok result = sarrayAppend(filename, sa);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief AppendRange() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa1).
 * Arg #2 is expected to be a Sarray* (sa2).
 * Arg #3 is expected to be a l_int32 (start).
 * Arg #4 is expected to be a l_int32 (end).
 *
 * Leptonica's Notes:
 *      (1) Copies of the strings in sarray2 are added to sarray1.
 *      (2) The [start ... end] range is truncated if necessary.
 *      (3) Use end == -1 to append to the end of sa2.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
AppendRange(lua_State *L)
{
    LL_FUNC("AppendRange");
    Sarray *sa1 = ll_check_Sarray(_fun, L, 1);
    Sarray *sa2 = ll_check_Sarray(_fun, L, 1);
    l_int32 start = ll_check_l_int32(_fun, L, 3);
    l_int32 end = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == sarrayAppendRange(sa1, sa2, start, end));
}

/**
 * \brief ChangeRefcount() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (delta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
ChangeRefcount(lua_State *L)
{
    LL_FUNC("ChangeRefcount");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == sarrayChangeRefcount(sa, delta));
}

/**
 * \brief Clear() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
Clear(lua_State *L)
{
    LL_FUNC("Clear");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == sarrayClear(sa));
}

/**
 * \brief Clone() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Clone(lua_State *L)
{
    LL_FUNC("Clone");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    Sarray *sad = sarrayClone(sa);
    return ll_push_Sarray(_fun, L, sad);
}

/**
 * \brief ConvertFilesFittedToPS() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_float32 (xpts).
 * Arg #3 is expected to be a l_float32 (ypts).
 * Arg #4 is expected to be a const char* (fileout).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
ConvertFilesFittedToPS(lua_State *L)
{
    LL_FUNC("ConvertFilesFittedToPS");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_float32 xpts = ll_check_l_float32(_fun, L, 2);
    l_float32 ypts = ll_check_l_float32(_fun, L, 3);
    const char *fileout = ll_check_string(_fun, L, 4);
    l_ok result = sarrayConvertFilesFittedToPS(sa, xpts, ypts, fileout);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief ConvertFilesToPS() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (res).
 * Arg #3 is expected to be a const char* (fileout).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
ConvertFilesToPS(lua_State *L)
{
    LL_FUNC("ConvertFilesToPS");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 res = ll_check_l_int32(_fun, L, 2);
    const char *fileout = ll_check_string(_fun, L, 3);
    l_ok result = sarrayConvertFilesToPS(sa, res, fileout);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief ConvertWordsToLines() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (linesize).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ConvertWordsToLines(lua_State *L)
{
    LL_FUNC("ConvertWordsToLines");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 linesize = ll_check_l_int32(_fun, L, 2);
    Sarray *sad = sarrayConvertWordsToLines(sa, linesize);
    return ll_push_Sarray(_fun, L, sad);
}

/**
 * \brief Copy() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    Sarray *sad = sarrayCopy(sa);
    return ll_push_Sarray(_fun, L, sad);
}

/**
 * \brief CreateInitialized() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (n).
 * Arg #2 is expected to be a string (initstr).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
CreateInitialized(lua_State *L)
{
    LL_FUNC("CreateInitialized");
    l_int32 n = ll_check_l_int32(_fun, L, 1);
    const char *str = ll_check_string(_fun, L, 2);
    char *initstr = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(str));
    Sarray *sa = sarrayCreateInitialized(n, initstr);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief CreateLinesFromString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const char* (string).
 * Arg #2 is expected to be a l_int32 (blankflag).
 *
 * Leptonica's Notes:
 *      (1) This finds the number of line substrings, each of which
 *          ends with a newline, and puts a copy of each substring
 *          in a new sarray.
 *      (2) The newline characters are removed from each substring.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
CreateLinesFromString(lua_State *L)
{
    LL_FUNC("CreateLinesFromString");
    const char *string = ll_check_string(_fun, L, 1);
    l_int32 blankflag = ll_check_l_int32(_fun, L, 2);
    Sarray *sa = sarrayCreateLinesFromString(string, blankflag);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief CreateWordsFromString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const char* (string).
 *
 * Leptonica's Notes:
 *      (1) This finds the number of word substrings, creates an sarray
 *          of this size, and puts copies of each substring into the sarray.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
CreateWordsFromString(lua_State *L)
{
    LL_FUNC("CreateWordsFromString");
    const char *string = ll_check_string(_fun, L, 1);
    Sarray *sa = sarrayCreateWordsFromString(string);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief FindStringByHash() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a DnaHash* (dahash).
 * Arg #3 is expected to be a const char* (str).
 *
 * Leptonica's Notes:
 *      (1) Fast lookup in dnaHash associated with a sarray, to see if a
 *          random string %str is already stored in the hash table.
 *      (2) We use a strong hash function to minimize the chance that
 *          two different strings hash to the same key value.
 *      (3) We select the number of buckets to be about 5% of the size
 *          of the input sarray, so that when fully populated, each
 *          bucket (dna) will have about 20 entries, each being an index
 *          into sa.  In lookup, after hashing to the key, and then
 *          again to the bucket, we traverse the bucket (dna), using the
 *          index into sa to check if %str has been found before.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
FindStringByHash(lua_State *L)
{
    LL_FUNC("FindStringByHash");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    DnaHash *dahash = ll_check_DnaHash(_fun, L, 2);
    const char *str = ll_check_string(_fun, L, 3);
    l_int32 index = 0;
    if (sarrayFindStringByHash(sa, dahash, str, &index))
        return ll_push_nil(L);
    ll_push_l_int32 (_fun, L, index);
    return 1;
}

/**
 * \brief GenerateIntegers() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (n).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
GenerateIntegers(lua_State *L)
{
    LL_FUNC("GenerateIntegers");
    l_int32 n = ll_check_l_int32(_fun, L, 1);
    Sarray *sa = sarrayGenerateIntegers(n);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief GetTable() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 on the Lua stack
 */
static int
TableArray(lua_State *L)
{
    LL_FUNC("TableArray");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    return ll_pack_Sarray(_fun, L, sa);
}

/**
 * \brief GetCount() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 result = sarrayGetCount(sa);
    return ll_push_l_int32 (_fun, L, result);
}

/**
 * \brief GetRefcount() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
GetRefcount(lua_State *L)
{
    LL_FUNC("GetRefcount");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 result = sarrayGetRefcount(sa);
    return ll_push_l_int32 (_fun, L, result);
}

/**
 * \brief GetString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (index).
 *
 * Leptonica's Notes:
 *      (1) See usage comments at the top of this file.
 *      (2) To get a pointer to the string itself, use L_NOCOPY.
 *          To get a copy of the string, use L_COPY.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
GetString(lua_State *L)
{
    LL_FUNC("GetString");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 index = ll_check_index(_fun, L, 2);
    char *result = sarrayGetString(sa, index, L_NOCOPY);
    return ll_push_string(_fun, L, result);
}

/**
 * \brief IntersectionByAset() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa1).
 * Arg #2 is expected to be a Sarray* (sa2).
 *
 * Leptonica's Notes:
 *      (1) Algorithm: put the larger sarray into a set, using the string
 *          hashes as the key values.  Then run through the smaller sarray,
 *          building an output sarray and a second set from the strings
 *          in the larger array: if a string is in the first set but
 *          not in the second, add the string to the output sarray and hash
 *          it into the second set.  The second set is required to make
 *          sure only one instance of each string is put into the output sarray.
 *          This is O(mlogn), {m,n} = sizes of {smaller,larger} input arrays.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
IntersectionByAset(lua_State *L)
{
    LL_FUNC("IntersectionByAset");
    Sarray *sa1 = ll_check_Sarray(_fun, L, 1);
    Sarray *sa2 = ll_check_Sarray(_fun, L, 2);
    Sarray *sa = sarrayIntersectionByAset(sa1, sa2);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief IntersectionByHash() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa1).
 * Arg #2 is expected to be a Sarray* (sa2).
 *
 * Leptonica's Notes:
 *      (1) This is faster than sarrayIntersectionByAset(), because the
 *          bucket lookup is O(n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
IntersectionByHash(lua_State *L)
{
    LL_FUNC("IntersectionByHash");
    Sarray *sa1 = ll_check_Sarray(_fun, L, 1);
    Sarray *sa2 = ll_check_Sarray(_fun, L, 2);
    Sarray *sa = sarrayIntersectionByHash(sa1, sa2);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief Join() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa1).
 * Arg #2 is expected to be a Sarray* (sa2).
 *
 * Leptonica's Notes:
 *      (1) Copies of the strings in sarray2 are added to sarray1.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    Sarray *sa1 = ll_check_Sarray(_fun, L, 1);
    Sarray *sa2 = ll_check_Sarray(_fun, L, 2);
    l_ok result = sarrayJoin(sa1, sa2);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief LookupCSKV() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a const string (keystring).
 *
 * Leptonica's Notes:
 *      (1) The input %sa can have other strings that are not in
 *          comma-separated key-value format.  These will be ignored.
 *      (2) This returns a copy of the first value string in %sa whose
 *          key string matches the input %keystring.
 *      (3) White space is not ignored; all white space before the ','
 *          is used for the keystring in matching.  This allows the
 *          key and val strings to have white space (e.g., multiple words).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
LookupCSKV(lua_State *L)
{
    LL_FUNC("LookupCSKV");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    const char *keystring = ll_check_string(_fun, L, 2);
    char *valstring = nullptr;
    if (sarrayLookupCSKV(sa, keystring, &valstring))
        return ll_push_nil(L);
    return ll_push_string(_fun, L, valstring);
}

/**
 * \brief PadToSameSize() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa1).
 * Arg #2 is expected to be a Sarray* (sa2).
 * Arg #3 is expected to be a char* (padstring).
 *
 * Leptonica's Notes:
 *      (1) If two sarrays have different size, this adds enough
 *          instances of %padstring to the smaller so that they are
 *          the same size.  It is useful when two or more sarrays
 *          are being sequenced in parallel, and it is necessary to
 *          find a valid string at each index.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
PadToSameSize(lua_State *L)
{
    LL_FUNC("PadToSameSize");
    Sarray *sa1 = ll_check_Sarray(_fun, L, 1);
    Sarray *sa2 = ll_check_Sarray(_fun, L, 2);
    const char *str = ll_check_string(_fun, L, 3);
    char *padstring = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(str));
    return ll_push_boolean(_fun, L, 0 == sarrayPadToSameSize(sa1, sa2, padstring));
}

/**
 * \brief ParseRange() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (start).
 * Arg #3 is expected to be a const char* (substr).
 * Arg #4 is expected to be a l_int32 (loc).
 *
 * Leptonica's Notes:
 *      (1) This finds the range of the next set of strings in SA,
 *          beginning the search at 'start', that does NOT have
 *          the substring 'substr' either at the indicated location
 *          in the string or anywhere in the string.  The input
 *          variable 'loc' is the specified offset within the string;
 *          use -1 to indicate 'anywhere in the string'.
 *      (2) Always check the return value to verify that a valid range
 *          was found.
 *      (3) If a valid range is not found, the values of actstart,
 *          end and newstart are all set to the size of sa.
 *      (4) If this is the last valid range, newstart returns the value n.
 *          In use, this should be tested before calling the function.
 *      (5) Usage example.  To find all the valid ranges in a file
 *          where the invalid lines begin with two dashes, copy each
 *          line in the file to a string in an sarray, and do:
 *             start = 0;
 *             while (!sarrayParseRange(sa, start, &actstart, &end, &start,
 *                    "--", 0))
 *                 fprintf(stderr, "start = %d, end = %d\n", actstart, end);
 * </pre>
 * \param L pointer to the lua_State
 * \return 3 on the Lua stack
 */
static int
ParseRange(lua_State *L)
{
    LL_FUNC("ParseRange");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 start = ll_check_l_int32(_fun, L, 2);
    const char *substr = ll_check_string(_fun, L, 3);
    l_int32 loc = ll_check_l_int32(_fun, L, 4);
    l_int32 actualstart = 0;
    l_int32 end = 0;
    l_int32 newstart = 0;
    sarrayParseRange(sa, start, &actualstart, &end, &newstart, substr, loc);
    ll_push_l_int32 (_fun, L, actualstart);
    ll_push_l_int32 (_fun, L, end);
    ll_push_l_int32 (_fun, L, newstart);
    return 3;
}

/**
 * \brief Read() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const char* (filename).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Sarray *sa = sarrayRead(filename);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief ReadMem() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const l_uint8* (data).
 * Arg #2 is expected to be a size_t (size).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
    Sarray *sa = sarrayReadMem(data, size);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief ReadStream() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (fp).
 *
 * Leptonica's Notes:
 *      (1) We store the size of each string along with the string.
 *          The limit on the number of strings is 2^24.
 *          The limit on the size of any string is 2^30 bytes.
 *      (2) This allows a string to have embedded newlines.  By reading
 *          the entire string, as determined by its size, we are
 *          not affected by any number of embedded newlines.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Sarray *sa = sarrayReadStream(stream->f);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief RemoveDupsByAset() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sas).
 *
 * Leptonica's Notes:
 *      (1) This is O(nlogn), considerably slower than
 *          sarrayRemoveDupsByHash() for large string arrays.
 *      (2) The key for each string is a 64-bit hash.
 *      (3) Build a set, using hashed strings as keys.  As the set is
 *          built, first do a find; if not found, add the key to the
 *          set and add the string to the output sarray.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
RemoveDupsByAset(lua_State *L)
{
    LL_FUNC("RemoveDupsByAset");
    Sarray *sas = ll_check_Sarray(_fun, L, 1);
    Sarray *sa = sarrayRemoveDupsByAset(sas);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief RemoveDupsByHash() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sas).
 *
 * Leptonica's Notes:
 *      (1) Generates a sarray with unique values.
 *      (2) The dnahash is built up with sad to assure uniqueness.
 *          It can be used to find if a string is in the set:
 *              sarrayFindValByHash(sad, dahash, str, &index)
 *      (3) The hash of the string location is simple and fast.  It scales
 *          up with the number of buckets to insure a fairly random
 *          bucket selection input strings.
 *      (4) This is faster than sarrayRemoveDupsByAset(), because the
 *          bucket lookup is O(n), although there is a double-loop
 *          lookup within the dna in each bucket.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 on the Lua stack
 */
static int
RemoveDupsByHash(lua_State *L)
{
    LL_FUNC("RemoveDupsByHash");
    Sarray *sas = ll_check_Sarray(_fun, L, 1);
    Sarray *sad = nullptr;
    DnaHash *dahash = nullptr;
    if (sarrayRemoveDupsByHash(sas, &sad, &dahash))
        return ll_push_nil(L);
    ll_push_Sarray(_fun, L, sad);
    ll_push_DnaHash (_fun, L, dahash);
    return 2;
}

/**
 * \brief RemoveString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (index).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
RemoveString(lua_State *L)
{
    LL_FUNC("RemoveString");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 index = ll_check_index(_fun, L, 2);
    char *result = sarrayRemoveString(sa, index);
    return ll_push_string(_fun, L, result);
}

/**
 * \brief ReplaceString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (index).
 * Arg #3 is expected to be a char* (newstr).
 * Arg #4 is expected to be a l_int32 (copyflag).
 *
 * Leptonica's Notes:
 *      (1) This destroys an existing string and replaces it with
 *          the new string or a copy of it.
 *      (2) By design, an sarray is always compacted, so there are
 *          never any holes (null ptrs) in the ptr array up to the
 *          current count.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
ReplaceString(lua_State *L)
{
    LL_FUNC("ReplaceString");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 index = ll_check_index(_fun, L, 2);
    const char* str = ll_check_string(_fun, L, 3);
    /** XXX: deconstify */
    char *newstr = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(str));
    return ll_push_boolean(_fun, L, 0 == sarrayReplaceString(sa, index, newstr, L_COPY));
}

/**
 * \brief SelectByRange() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sain).
 * Arg #2 is expected to be a l_int32 (first).
 * Arg #3 is expected to be a l_int32 (last).
 *
 * Leptonica's Notes:
 *      (1) This makes %saout consisting of copies of all strings in %sain
 *          in the index set [first ... last].  Use %last == 0 to get all
 *          strings from %first to the last string in the sarray.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
SelectByRange(lua_State *L)
{
    LL_FUNC("SelectByRange");
    Sarray *sain = ll_check_Sarray(_fun, L, 1);
    l_int32 first = ll_check_l_int32(_fun, L, 2);
    l_int32 last = ll_check_l_int32(_fun, L, 3);
    Sarray *sa = sarraySelectByRange(sain, first, last);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief SelectBySubstring() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sain).
 * Arg #2 is expected to be a const char* (substr).
 *
 * Leptonica's Notes:
 *      (1) This selects all strings in sain that have substr as a substring.
 *          Note that we can't use strncmp() because we're looking for
 *          a match to the substring anywhere within each filename.
 *      (2) If substr == NULL, returns a copy of the sarray.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
SelectBySubstring(lua_State *L)
{
    LL_FUNC("SelectBySubstring");
    Sarray *sain = ll_check_Sarray(_fun, L, 1);
    const char *substr = ll_check_string(_fun, L, 2);
    Sarray *sa = sarraySelectBySubstring(sain, substr);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief Sort() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (saout).
 * Arg #2 is expected to be a Sarray* (sain).
 * Arg #3 is expected to be a l_int32 (sortorder).
 *
 * Leptonica's Notes:
 *      (1) Set saout = sain for in-place; otherwise, set naout = NULL.
 *      (2) Shell sort, modified from K&R, 2nd edition, p.62.
 *          Slow but simple O(n logn) sort.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Sort(lua_State *L)
{
    LL_FUNC("Sort");
    Sarray *saout = ll_check_Sarray(_fun, L, 1);
    Sarray *sain = ll_check_Sarray(_fun, L, 2);
    l_int32 sortorder = ll_check_l_int32(_fun, L, 3);
    Sarray *sa = sarraySort(saout, sain, sortorder);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief SortByIndex() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sain).
 * Arg #2 is expected to be a Numa* (naindex).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
SortByIndex(lua_State *L)
{
    LL_FUNC("SortByIndex");
    Sarray *sain = ll_check_Sarray(_fun, L, 1);
    Numa *naindex = ll_check_Numa(_fun, L, 2);
    Sarray *sa = sarraySortByIndex(sain, naindex);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief SplitString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a const char* (str).
 * Arg #3 is expected to be a const char* (separators).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
SplitString(lua_State *L)
{
    LL_FUNC("SplitString");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    const char *str = ll_check_string(_fun, L, 2);
    const char *separators = ll_check_string(_fun, L, 3);
    l_int32 result = sarraySplitString(sa, str, separators);
    return ll_push_l_int32 (_fun, L, result);
}

/**
 * \brief ToString() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (addnlflag).
 *
 * Leptonica's Notes:
 *      (1) Concatenates all the strings in the sarray, preserving
 *          all white space.
 *      (2) If addnlflag != 0, adds either a '\n' or a ' ' after
 *          each substring.
 *      (3) This function was NOT implemented as:
 *            for (i = 0; i < n; i++)
 *                     strcat(dest, sarrayGetString(sa, i, L_NOCOPY));
 *          Do you see why?
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ToString(lua_State *L)
{
    LL_FUNC("ToString");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 addnlflag = ll_opt_boolean(_fun, L, 2, FALSE);
    char *str = sarrayToString(sa, addnlflag);
    ll_push_string(_fun, L, str);
    ll_free(str);
    return 1;
}

/**
 * \brief ToStringRange() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 * Arg #2 is expected to be a l_int32 (first).
 * Arg #3 is expected to be a l_int32 (nstrings).
 * Arg #4 is expected to be a l_int32 (addnlflag).
 *
 * Leptonica's Notes:
 *      (1) Concatenates the specified strings inthe sarray, preserving
 *          all white space.
 *      (2) If addnlflag != 0, adds either a '\n' or a ' ' after
 *          each substring.
 *      (3) If the sarray is empty, this returns a string with just
 *          the character corresponding to %addnlflag.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ToStringRange(lua_State *L)
{
    LL_FUNC("ToStringRange");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_int32 first = ll_check_l_int32(_fun, L, 2);
    l_int32 nstrings = ll_check_l_int32(_fun, L, 3);
    l_int32 addnlflag = ll_opt_boolean(_fun, L, 4, FALSE);
    char *str = sarrayToStringRange(sa, first, nstrings, addnlflag);
    ll_push_string(_fun, L, str);
    ll_free(str);
    return 1;
}

/**
 * \brief UnionByAset() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa1).
 * Arg #2 is expected to be a Sarray* (sa2).
 *
 * Leptonica's Notes:
 *      (1) Duplicates are removed from the concatenation of the two arrays.
 *      (2) The key for each string is a 64-bit hash.
 *      (2) Algorithm: Concatenate the two sarrays.  Then build a set,
 *          using hashed strings as keys.  As the set is built, first do
 *          a find; if not found, add the key to the set and add the string
 *          to the output sarray.  This is O(nlogn).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
UnionByAset(lua_State *L)
{
    LL_FUNC("UnionByAset");
    Sarray *sa1 = ll_check_Sarray(_fun, L, 1);
    Sarray *sa2 = ll_check_Sarray(_fun, L, 2);
    Sarray *sa = sarrayUnionByAset(sa1, sa2);
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief Write() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const char* (filename).
 * Arg #2 is expected to be a Sarray* (sa).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_ok result = sarrayWrite(filename, sa);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief WriteMem() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sarray* (sa).
 *
 * Leptonica's Notes:
 *      (1) Serializes a sarray in memory and puts the result in a buffer.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 lstring on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (sarrayWriteMem(&data, &size, sa))
        return ll_push_nil(L);
    ll_push_bytes(_fun, L, data, size);
    return 1;
}

/**
 * \brief WriteStream() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (fp).
 * Arg #2 is expected to be a Sarray* (sa).
 *
 * Leptonica's Notes:
 *      (1) This appends a '\n' to each string, which is stripped
 *          off by sarrayReadStream().
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Sarray *sa = ll_check_Sarray(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_ok result = sarrayWriteStream(stream->f, sa);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief Check Lua stack at index %arg for udata of class Sarray*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Sarray* contained in the user data
 */
Sarray *
ll_check_Sarray(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Sarray>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Sarray* contained in the user data
 */
Sarray *
ll_opt_Sarray(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Sarray(_fun, L, arg);
}

/**
 * \brief Push Sarray user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param sa pointer to the Sarray
 * \return 1 Sarray* on the Lua stack
 */
int
ll_push_Sarray(const char *_fun, lua_State *L, Sarray *sa)
{
    if (!sa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, sa);
}

/**
 * \brief Create and push a new Sarray*.
 * \param L pointer to the lua_State
 * \return 1 Sarray* on the Lua stack
 */
int
ll_new_Sarray(lua_State *L)
{
    FUNC("ll_new_Sarray");
    Sarray *sa = nullptr;
    const char* str = nullptr;
    char* src = nullptr;
    l_int32 n = 1;

    if (ll_istable(_fun, L, 1)) {
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %d\n", _fun,
            "table", 1);
        sa = ll_unpack_Sarray(_fun, L, 1, nullptr);
    }

    if (ll_isstring(_fun, L, 1)) {
        str = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            "const char", reinterpret_cast<const void *>(src));
        sa = sarrayCreate(1);
        src = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(str));
        sarrayAddString(sa, src, L_COPY);
    }

    if (!sa && ll_isinteger(_fun, L, 1)) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d\n", _fun,
            "n", n);
        sa = sarrayCreate(n);
    }
    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(sa));
    return ll_push_Sarray(_fun, L, sa);
}

/**
 * \brief Register the BMF methods and functions in the TNAME meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
luaopen_Sarray(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Sarray},
        {"__len",                   GetCount},
        {"__tostring",              toString},
        {"AddString",               AddString},
        {"Append",                  Append},
        {"AppendRange",             AppendRange},
        {"ChangeRefcount",          ChangeRefcount},
        {"Clear",                   Clear},
        {"Clone",                   Clone},
        {"ConvertFilesFittedToPS",  ConvertFilesFittedToPS},
        {"ConvertFilesToPS",        ConvertFilesToPS},
        {"ConvertWordsToLines",     ConvertWordsToLines},
        {"Copy",                    Copy},
        {"Create",                  Create},
        {"CreateInitialized",       CreateInitialized},
        {"CreateLinesFromString",   CreateLinesFromString},
        {"CreateWordsFromString",   CreateWordsFromString},
        {"Destroy",                 Destroy},
        {"FindStringByHash",        FindStringByHash},
        {"GenerateIntegers",        GenerateIntegers},
        {"GetCount",                GetCount},
        {"GetRefcount",             GetRefcount},
        {"GetString",               GetString},
        {"IntersectionByAset",      IntersectionByAset},
        {"IntersectionByHash",      IntersectionByHash},
        {"Join",                    Join},
        {"LookupCSKV",              LookupCSKV},
        {"PadToSameSize",           PadToSameSize},
        {"ParseRange",              ParseRange},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RemoveDupsByAset",        RemoveDupsByAset},
        {"RemoveDupsByHash",        RemoveDupsByHash},
        {"RemoveString",            RemoveString},
        {"ReplaceString",           ReplaceString},
        {"SelectByRange",           SelectByRange},
        {"SelectBySubstring",       SelectBySubstring},
        {"Sort",                    Sort},
        {"SortByIndex",             SortByIndex},
        {"SplitString",             SplitString},
        {"TableArray",              TableArray},
        {"ToString",                ToString},
        {"ToStringRange",           ToStringRange},
        {"UnionByAset",             UnionByAset},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_global_cfunct(_fun, L, TNAME, ll_new_Sarray);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
