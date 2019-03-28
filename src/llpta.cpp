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
 * \file llpta.cpp
 * \class Pta
 *
 * An array of points (l_float32 x and y).
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PTA

/** Define a function's name (_fun) with prefix Pta */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Pta*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 *
 * Leptonica's Notes:
 *      (1) Decrements the ref count and, if 0, destroys the pta.
 *      (2) Always nulls the input ptr.
 * </pre>
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Pta *pta = ll_take_udata<Pta>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d, %s = %d\n", _fun,
        TNAME,
        "pta", reinterpret_cast<void *>(pta),
        "count", ptaGetCount(pta),
        "refcount", ptaGetRefcount(pta));
    ptaDestroy(&pta);
    return 0;
}

/**
 * \brief Create a new Pta*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L Lua state.
 * \return 1 Pta* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Pta *pa = ptaCreate(n);
    return ll_push_Pta(_fun, L, pa);
}

/**
 * \brief Get the number of stored numbers in the Pta*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    ll_push_l_int32(_fun, L, ptaGetCount(pta));
    return 1;
}

/**
 * \brief Printable string for a Pta* (%pta).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Pta *pta = ll_check_Pta(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!pta) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(pta));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        for (l_int32 i = 0; i < ptaGetCount(pta); i++) {
            l_float32 px, py;
            if (ptaGetPt(pta, i, &px, &py)) {
                snprintf(str, LL_STRBUFF,
                         "\n    %d: <invalid>",
                         i+1);
            } else {
                snprintf(str, sizeof(str),
                         "\n    %d: %s = %g, %s = %g",
                         i + 1,
                         "px", static_cast<double>(px),
                         "py", static_cast<double>(py));
            }
            luaL_addstring(&B, str);
        }
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Add one number to the Pta*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * Arg #2 is expected to be a l_float32 (x).
 * Arg #3 is expected to be a l_float32 (y).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AddPt(lua_State *L)
{
    LL_FUNC("AddPt");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_float32 x = ll_check_l_float32(_fun, L, 2);
    l_float32 y = ll_check_l_float32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == ptaAddPt(pta, x, y));
}

/**
 * \brief Clone a Pta*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 Pta* on the Lua stack.
 */
static int
Clone(lua_State *L)
{
    LL_FUNC("Clone");
    Pta *ptas = ll_check_Pta(_fun, L, 1);
    Pta *pta = ptaClone(ptas);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Convert the Pta* (%pta) to a Box* (%box).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 *
 * Leptonica's Notes:
 *      (1) For 2 corners, the order of the 2 points is UL, LR.
 *          For 4 corners, the order of points is UL, UR, LL, LR.
 * </pre>
 * \param L Lua state.
 * \return 2 for two Numa* on the Lua stack, or 0 in case of error.
 */
static int
ConvertToBox(lua_State *L)
{
    LL_FUNC("ConvertToBox");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    Box *box = ptaConvertToBox(pta);
    return ll_push_Box(_fun, L, box);
}

/**
 * \brief Copy a Pta* (%ptas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 Pta* on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Pta *ptas = ll_check_Pta(_fun, L, 1);
    Pta *pta = ptaCopy(ptas);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Copy a range (%istart, %iend) from a Pta* (%ptas).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * Arg #2 is expected to be a l_int32 (istart).
 * Arg #3 is expected to be a l_int32 (uend).
 * </pre>
 * \param L Lua state.
 * \return 1 Pta* on the Lua stack.
 */
static int
CopyRange(lua_State *L)
{
    LL_FUNC("CopyRange");
    Pta *ptas = ll_check_Pta(_fun, L, 1);
    l_int32 istart = ll_check_index(_fun, L, 2, ptaGetCount(ptas));
    l_int32 iend = ll_check_index(_fun, L, 3, ptaGetCount(ptas));
    Pta *pta = ptaCopyRange(ptas, istart, iend);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Set the number of stored numbes in the Pta* to zero.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 *
 * Leptonica's Notes:
 *      This only resets the Pta::n field, for reuse
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Empty(lua_State *L)
{
    LL_FUNC("Empty");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == ptaEmpty(pta));
}

/**
 * \brief Get the Pta* (%pta) as an two Numa* (%ptax, %ptay) for X and Y.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 *
 * Leptonica's Notes:
 *      (1) This copies the internal arrays into new Numas.
 * </pre>
 * \param L Lua state.
 * \return 2 for two Numa* on the Lua stack, or 0 in case of error.
 */
static int
GetArrays(lua_State *L)
{
    LL_FUNC("GetArrays");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    Numa *ptax = nullptr;
    Numa *ptay = nullptr;
    if (ptaGetArrays(pta, &ptax, &ptay))
        return ll_push_nil(_fun, L);
    return ll_push_Numa(_fun, L, ptax) + ll_push_Numa(_fun, L, ptay);
}

/**
 * \brief Get the Point from the Pta* (%pta) at index (%idx) as two Lua integers.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetIPt(lua_State *L)
{
    LL_FUNC("GetIPt");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, ptaGetCount(pta));
    l_int32 x;
    l_int32 y;
    if (ptaGetIPt(pta, idx, &x, &y))
        return ll_push_nil(_fun, L);
    ll_push_l_int32(_fun, L, x);
    ll_push_l_int32(_fun, L, y);
    return 2;
}

/**
 * \brief Get the Point from the Pta* (%pta) at index (%idx) as two Lua numbers.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L Lua state.
 * \return 1 integer on the Lua stack.
 */
static int
GetPt(lua_State *L)
{
    LL_FUNC("GetPt");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, ptaGetCount(pta));
    l_float32 x = 0;
    l_float32 y = 0;
    if (ptaGetPt(pta, idx, &x, &y))
        return ll_push_nil(_fun, L);
    ll_push_l_float32(_fun, L, x);
    ll_push_l_float32(_fun, L, y);
    return 2;
}

/**
 * \brief Insert one number to the Pta* (%pta) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
InsertPt(lua_State *L)
{
    LL_FUNC("InsertPt");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, ptaGetCount(pta));
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == ptaInsertPt(pta, idx, x, y));
}

/**
 * \brief Read a Pta* from an external file.
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 * </pre>
 * \param L Lua state.
 * \return 1 Ptaa* on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Pta *pta = ptaRead(filename);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Read a Ptaa* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L Lua state.
 * \return 1 Ptaa* on the Lua stack.
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Pta *pta = ptaReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Read a Pta* from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 1 Ptaa* on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pta *pta = ptaReadStream(stream->f);
    return ll_push_Pta(_fun, L, pta);
}

/**
 * \brief Remove one number from the Pta* (%pta) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 *
 * Leptonica's Notes:
 *      (1) This shifts pta[i] --> pta[i - 1] for all i > index.
 *      (2) It should not be used repeatedly on large arrays,
 *          because the function is O(n).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
RemovePt(lua_State *L)
{
    LL_FUNC("RemovePt");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, ptaGetCount(pta));
    return ll_push_boolean(_fun, L, 0 == ptaRemovePt(pta, idx));
}

/**
 * \brief Set the values for the Pta* (%pta) at the given index (%idx).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_float32 (x).
 * Arg #4 is expected to be a l_float32 (y).
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
SetPt(lua_State *L)
{
    LL_FUNC("SetPt");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, ptaGetCount(pta));
    l_float32 x = ll_check_l_float32(_fun, L, 3);
    l_float32 y = ll_check_l_float32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == ptaSetPt(pta, idx, x, y));
}

/**
 * \brief Write the Pta* (%pta) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is expected to be string containing the filename.
 * Arg #3 is an optional boolean (type)
 *
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    l_int32 type = ll_opt_boolean(_fun, L, 3, FALSE);
    return ll_push_boolean(_fun, L, 0 == ptaWrite(filename, pta, type));
}

/**
 * \brief Write the Pta* (%pta) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is an optional boolean (type)
 *
 * Leptonica's Notes:
 *      (1) Serializes a pta in memory and puts the result in a buffer.
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_int32 type = ll_opt_boolean(_fun, L, 2, FALSE);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (ptaWriteMem(&data, &size, pta, type))
        return ll_push_nil(_fun, L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write the Pta* (%pta) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Ptaa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is an optional boolean (type)
 *
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 type = ll_opt_boolean(_fun, L, 3, FALSE);
    return ll_push_boolean(_fun, L, 0 == ptaWriteStream(stream->f, pta, type));
}

/**
 * \brief Check Lua stack at index %arg for user data of class Pta*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pta* contained in the user data.
 */
Pta *
ll_check_Pta(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Pta>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Pta* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pta* contained in the user data.
 */
Pta *
ll_opt_Pta(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Pta(_fun, L, arg);
}

/**
 * \brief Push PTA user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param pta pointer to the PTA
 * \return 1 Pta* on the Lua stack.
 */
int
ll_push_Pta(const char *_fun, lua_State *L, Pta *pta)
{
    if (!pta)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, pta);
}

/**
 * \brief Create and push a new Pta*.
 * \param L Lua state.
 * \return 1 Pta* on the Lua stack.
 */
int
ll_new_Pta(lua_State *L)
{
    FUNC("ll_new_Pta");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    Pta *pa = ptaCreate(n);
    return ll_push_Pta(_fun, L, pa);
}

/**
 * \brief Register the Pta methods and functions in the Pta meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Pta(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",            Destroy},
        {"__new",           ll_new_Pta},
        {"__len",           GetCount},
        {"__tostring",      toString},
        {"AddPt",           AddPt},
        {"Clone",           Clone},
        {"ConvertToBox",    ConvertToBox},
        {"Copy",            Copy},
        {"CopyRange",       CopyRange},
        {"Create",          Create},
        {"Destroy",         Destroy},
        {"Empty",           Empty},
        {"GetArrays",       GetArrays},
        {"GetIPt",          GetIPt},
        {"GetPt",           GetPt},
        {"InsertPt",        InsertPt},
        {"Read",            Read},
        {"ReadMem",         ReadMem},
        {"ReadStream",      ReadStream},
        {"RemovePt",        RemovePt},
        {"SetPt",           SetPt},
        {"Write",           Write},
        {"WriteMem",        WriteMem},
        {"WriteStream",     WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Pta);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
