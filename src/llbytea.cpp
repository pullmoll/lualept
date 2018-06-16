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
 * \file llbytea.cpp
 * \class Bytea
 *
 * An array of bytes.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_BYTEA

/** Define a function's name (_fun) with prefix Bytea */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Bytea*.
 *
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    L_Bytea *ba = ll_take_udata<L_Bytea>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "ba", reinterpret_cast<void *>(ba));
    l_byteaDestroy(&ba);
    return 0;
}

/**
 * \brief Create a new Bytea* (%ba).
 * <pre>
 * Arg #1 is expected to be a size_t (nbytes).
 *
 * Leptonica's Notes:
 *      (1) The allocated array is n + 1 bytes.  This allows room
 *          for null termination.
 * </pre>
 * \param L Lua state.
 * \return 1 Bytea* (%ba) on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    size_t nbytes = ll_check_l_uint32(_fun, L, 1);
    Bytea *ba = l_byteaCreate(nbytes);
    return ll_push_Bytea(_fun, L, ba);
}

/**
 * \brief Get the size of the Bytea* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 size_t on the Lua stack.
 */
static int
GetSize(lua_State *L)
{
    LL_FUNC("GetSize");
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    size_t size = l_byteaGetSize(ba);
    return ll_push_size_t(_fun, L, size);
}

/**
 * \brief Printable string for a Bytea*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea*.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!ba) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(ba));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF, "\n    %-14s: %" PRIu64,
                 "nalloc", static_cast<l_uintptr_t>(ba->nalloc));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %" PRIu64,
                 "size", static_cast<l_uintptr_t>(ba->size));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "refcount", ba->refcount);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "data", "void", reinterpret_cast<void *>(ba->data));
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Append data (%newdata, %newbytes) to the Bytea* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba).
 * Arg #2 is expected to be a lstring (newdata, newbytes).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AppendData(lua_State *L)
{
    LL_FUNC("AppendData");
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    size_t newbytes = 0;
    const l_uint8 *newdata = ll_check_lbytes(_fun, L, 2, &newbytes);
    return ll_push_boolean(_fun, L, 0 == l_byteaAppendData(ba, newdata, newbytes));
}

/**
 * \brief Append a string (%str) to the Bytea* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba).
 * Arg #2 is expected to be a char* (str).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
AppendString(lua_State *L)
{
    LL_FUNC("AppendString");
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    const char *str = ll_check_string(_fun, L, 2);
    l_ok ok = l_byteaAppendString(ba, str);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Copy the Bytea* (%bas) to a new Bytea* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (bas).
 * Arg #2 is expected to be a l_int32 (copyflag).
 *
 * Leptonica's Notes:
 *      (1) If cloning, up the refcount and return a ptr to %bas.
 * </pre>
 * \param L Lua state.
 * \return 1 on the Lua stack.
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Bytea *bas = ll_check_Bytea(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    Bytea *ba = l_byteaCopy(bas, copyflag);
    return ll_push_Bytea(_fun, L, ba);
}

/**
 * \brief Copy the data (%data, %size) from a Bytea* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba).
 *
 * Leptonica's Notes:
 *      (1) The returned data is owned by the caller.  The input %ba
 *          still owns the original data array.
 * </pre>
 * \param L Lua state.
 * \return 1 lstring on the Lua stack.
 */
static int
CopyData(lua_State *L)
{
    LL_FUNC("CopyData");
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    size_t size = 0;
    l_uint8 *data = l_byteaCopyData(ba, &size);
    ll_push_bytes(_fun, L, data, size);
    ll_free(data);
    return 1;
}


/**
 * \brief Find each sequence (%sequence, %seqlen) in the Bytea* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba).
 * Arg #2 is expected to be a lstring (sequence, seqlen).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 Dna* (%da) on the Lua stack.
 */
static int
FindEachSequence(lua_State *L)
{
    LL_FUNC("FindEachSequence");
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    size_t seqlen = 0;
    const l_uint8 *sequence = ll_check_lbytes(_fun, L, 2, &seqlen);
    Dna *da = nullptr;
    if (l_byteaFindEachSequence(ba, sequence, seqlen, &da))
        return ll_push_nil(_fun, L);
    ll_push_Dna(_fun, L, da);
    return 1;
}

/**
 * \brief Get the data (%data, %size) from a Bytea* (%ba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba).
 *
 * Leptonica's Notes:
 *      (1) The returned ptr is owned by %ba.  Do not free it!
 * </pre>
 * \param L Lua state.
 * \return 1 lstring (%data, %size) on the Lua stack.
 */
static int
GetData(lua_State *L)
{
    LL_FUNC("GetData");
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    size_t size = 0;
    l_uint8 *data = l_byteaGetData(ba, &size);
    ll_push_bytes(_fun, L, data, size);
    return 1;
}

/**
 * \brief Initialize the Bytea* (%ba) from a file (%fname).
 * <pre>
 * Arg #1 is expected to be a const char* (fname).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 Bytea* (%ba) on the Lua stack.
 */
static int
InitFromFile(lua_State *L)
{
    LL_FUNC("InitFromFile");
    const char *fname = ll_check_string(_fun, L, 1);
    Bytea *ba = l_byteaInitFromFile(fname);
    return ll_push_Bytea(_fun, L, ba);
}

/**
 * \brief Initialize the Bytea* (%ba) from memory (%data, %size).
 * <pre>
 * Arg #1 is expected to be a lstring (data, size).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 Bytea* (%ba) on the Lua stack.
 */
static int
InitFromMem(lua_State *L)
{
    LL_FUNC("InitFromMem");
    size_t size = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 1, &size);
    Bytea *ba = l_byteaInitFromMem(data, size);
    return ll_push_Bytea(_fun, L, ba);
}

/**
 * \brief Initialize the Bytea* (%ba) from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream->f).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 Bytea* (%ba) on the Lua stack.
 */
static int
InitFromStream(lua_State *L)
{
    LL_FUNC("InitFromStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Bytea *ba = l_byteaInitFromStream(stream->f);
    return ll_push_Bytea(_fun, L, ba);
}

/**
 * \brief Join Bytea* (%ba2) to Bytea* (%ba1).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba1).
 * Arg #2 is expected to be a Bytea* (ba2).
 *
 * Leptonica's Notes:
 *      (1) It is a no-op, not an error, for %ba2 to be null.
 * </pre>
 * \param L Lua state.
 * \return 1 Bytea* (%ba2) on the Lua stack.
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    Bytea *ba1 = ll_check_Bytea(_fun, L, 1);
    Bytea *ba2 = ll_opt_Bytea(_fun, L, 2);
    if (l_byteaJoin(ba1, &ba2))
        return ll_push_nil(_fun, L);
    ll_push_Bytea(_fun, L, ba2);
    return 1;
}

/**
 * \brief Split Bytea* (%ba1) at location (%splitloc) giving a new Bytea* (%ba2).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba1).
 * Arg #2 is expected to be a size_t (splitloc).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 Bytea* (%ba2) on the Lua stack.
 */
static int
Split(lua_State *L)
{
    LL_FUNC("Split");
    Bytea *ba1 = ll_check_Bytea(_fun, L, 1);
    size_t splitloc = ll_check_size_t(_fun, L, 2);
    Bytea *ba2 = nullptr;
    if (l_byteaSplit(ba1, splitloc, &ba2))
        return ll_push_nil(_fun, L);
    ll_push_Bytea(_fun, L, ba2);
    return 1;
}

/**
 * \brief Write a Bytea* (%ba) to an external file (%fname).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba).
 * Arg #2 is expected to be a const char* (fname).
 * Arg #3 is expected to be a size_t (startloc).
 * Arg #4 is expected to be a size_t (endloc).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 boolean on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    const char *fname = ll_check_string(_fun, L, 2);
    size_t startloc = ll_opt_size_t(_fun, L, 3, 0);
    size_t endloc = ll_opt_size_t(_fun, L, 4, l_byteaGetSize(ba) - 1);
    return ll_push_boolean(_fun, L, 0 == l_byteaWrite(fname, ba, startloc, endloc));
}

/**
 * \brief Write a Bytea* (%ba) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Bytea* (ba).
 * Arg #2 is expected to be a luaL_Stream* (stream->f).
 * Arg #3 is expected to be a size_t (startloc).
 * Arg #4 is expected to be a size_t (endloc).
 *
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Bytea *ba = ll_check_Bytea(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    size_t startloc = ll_opt_size_t(_fun, L, 3, 0);
    size_t endloc = ll_opt_size_t(_fun, L, 4, l_byteaGetSize(ba) - 1);
    l_ok ok = l_byteaWriteStream(stream->f, ba, startloc, endloc);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Check Lua stack at index %arg for user data of class Bytea*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Bytea* contained in the user data.
 */
L_Bytea *
ll_check_Bytea(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<L_Bytea>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Bytea* contained in the user data.
 */
L_Bytea *
ll_opt_Bytea(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Bytea(_fun, L, arg);
}
/**
 * \brief Push Bytea* user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param bmf pointer to the L_Bytea
 * \return 1 Bytea* on the Lua stack.
 */
int
ll_push_Bytea(const char *_fun, lua_State *L, L_Bytea *bmf)
{
    if (!bmf)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, bmf);
}
/**
 * \brief Create and push a new Bytea*.
 * \param L Lua state.
 * \return 1 Bytea* on the Lua stack.
 */
int
ll_new_Bytea(lua_State *L)
{
    FUNC("ll_new_Bytea");
    Bytea *ba = nullptr;
    size_t nbytes = 1;

    DBG(LOG_NEW_PARAM, "%s: create for %s = %u\n", _fun,
        "nbytes", nbytes);
    ba = l_byteaCreate(nbytes);
    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(ba));
    return ll_push_Bytea(_fun, L, ba);
}
/**
 * \brief Register the Bytea methods and functions in the TNAME meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Bytea(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_Bytea},
        {"__len",               GetSize},
        {"__tostring",          toString},
        {"AppendData",          AppendData},
        {"AppendString",        AppendString},
        {"Copy",                Copy},
        {"CopyData",            CopyData},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"FindEachSequence",    FindEachSequence},
        {"GetData",             GetData},
        {"GetSize",             GetSize},
        {"InitFromFile",        InitFromFile},
        {"InitFromMem",         InitFromMem},
        {"InitFromStream",      InitFromStream},
        {"Join",                Join},
        {"Split",               Split},
        {"Write",               Write},
        {"WriteStream",         WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Bytea);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
