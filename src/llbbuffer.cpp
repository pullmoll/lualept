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
 * \file llbbuffer.cpp
 * \class ByteBuffer
 *
 * Asynchronous reading and writing of data from / to a memory array of bytes.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_BBUFFER

/** Define a function's name (_fun) with prefix ByteBuffer */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a ByteBuffer* (bbuffer).
 *
 * Leptonica's Notes:
 *      (1) Destroys the byte array in the bbuffer and then the bbuffer;
 *          then nulls the contents of the input ptr.
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    ByteBuffer *bb = ll_take_udata<ByteBuffer>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "bb", reinterpret_cast<void *>(bb));
    bbufferDestroy(&bb);
    return 0;
}

/**
 * \brief Printable string for a ByteBuffer*.
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    ByteBuffer *bb = ll_check_ByteBuffer(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!bb) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF, TNAME "*: %p", reinterpret_cast<void *>(bb));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF, "\n    %s = 0x%x, %s = 0x%x, %s = 0x%x",
                 "nalloc", bb->nalloc,
                 "n", bb->n,
                 "nwritten", bb->nwritten);
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Create a new ByteBuffer* (%bb).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a lstring (data).
 * Arg #2 is expected to be a l_int32 (nalloc).
 *
 * Leptonica's Notes:
 *      (1) If a buffer address is given, you should read all the data in.
 *      (2) Allocates a bbuffer with associated byte array of
 *          the given size.  If a buffer address is given,
 *          it then reads the number of bytes into the byte array.
 * </pre>
 * \param L Lua state.
 * \return 1 ByteBuffer* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 nalloc = 0;
    const l_uint8 *indata = ll_check_lbytes(_fun, L, 1, &nalloc);
    ByteBuffer *bb = bbufferCreate(indata, nalloc);
    return ll_push_ByteBuffer(_fun, L, bb);
}

/**
 * \brief Destroy a ByteBuffer* (%bb) and save its data (%bytes, %nbytes).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a ByteBuffer* (bb).
 *
 * Leptonica's Notes:
 *      (1) Copies data to newly allocated array; then destroys the bbuffer.
 * </pre>
 * \param L Lua state.
 * \return 1 lstring on the Lua stack.
 */
static int
DestroyAndSaveData(lua_State *L)
{
    LL_FUNC("DestroyAndSaveData");
    ByteBuffer *bb = ll_check_ByteBuffer(_fun, L, 1);
    size_t nbytes = 0;
    l_uint8 *bytes = bbufferDestroyAndSaveData(&bb, &nbytes);
    ll_push_bytes(_fun, L, bytes, nbytes);
    return 1;
}

/**
 * \brief Extend the array of a ByteBuffer* (%bb)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a ByteBuffer* (bb).
 * Arg #2 is expected to be a l_int32 (nbytes).
 *
 * Leptonica's Notes:
 *      (1) reallocNew() copies all bb->nalloc bytes, even though
 *          only bb->n are data.
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
ExtendArray(lua_State *L)
{
    LL_FUNC("ExtendArray");
    ByteBuffer *bb = ll_check_ByteBuffer(_fun, L, 1);
    l_int32 nbytes = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == bbufferExtendArray(bb, nbytes));
}

/**
 * \brief Read a ByteBuffer* (%bb) from memory (%data, %nbytes).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a ByteBuffer* (bb).
 * Arg #2 is expected to be a lstring (data).
 *
 * Leptonica's Notes:
 *      (1) For a read after write, first remove the written
 *          bytes by shifting the unwritten bytes in the array,
 *          then check if there is enough room to add the new bytes.
 *          If not, realloc with bbufferExpandArray(), resulting
 *          in a second writing of the unwritten bytes.  While less
 *          efficient, this is simpler than making a special case
 *          of reallocNew().
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    ByteBuffer *bb = ll_check_ByteBuffer(_fun, L, 1);
    l_int32 nbytes = 0;
    const l_uint8 *data = ll_check_lbytes(_fun, L, 2, &nbytes);
    /* XXX: deconstify */
    l_uint8 *src = reinterpret_cast<l_uint8 *>(reinterpret_cast<l_intptr_t>(data));
    l_ok result = bbufferRead(bb, src, nbytes);
    return ll_push_boolean(_fun, L, 0 == result);
}

/**
 * \brief Read a ByteBuffer* (%bb) from a Lua io stream (%stream).
 * Read all data from %stream from the current position until its end.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a ByteBuffer* (bb).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L Lua state.
 * \return 0 on the Lua stack.
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    ByteBuffer *bb = ll_check_ByteBuffer(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 nbytes = 0;
    long pos = ftell(stream->f);
    if (fseek(stream->f, 0, SEEK_END))
        return ll_push_boolean(_fun, L, FALSE);
    nbytes = static_cast<l_int32>(ftell(stream->f) - pos);
    fseek(stream->f, pos, SEEK_SET);
    return ll_push_boolean(_fun, L, 0 == bbufferReadStream(bb, stream->f, nbytes));
}

/**
 * \brief Write ByteBuffer* (%bb) data to a lstring (%dest, %nout).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a ByteBuffer* (bb).
 * Arg #2 is expected to be a size_t (nbytes).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 lstring (%bytes, %nout) on the Lua stack.
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    ByteBuffer *bb = ll_check_ByteBuffer(_fun, L, 1);
    size_t nbytes = ll_check_size_t(_fun, L, 2);
    l_uint8 *dest = ll_calloc<l_uint8>(_fun, L, nbytes);
    size_t nout = 0;
    if (bbufferWrite(bb, dest, nbytes, &nout))
        return ll_push_nil(_fun, L);
    ll_push_bytes(_fun, L, dest, nout);
    return 1;
}

/**
 * \brief Write a number of bytes from ByteBuffer* (%bb) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a ByteBuffer* (bb).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * Arg #3 is expected to be a size_t (nbytes).
 *
 * </pre>
 * \param L Lua state.
 * \return 1 size_t (%nout) on the Lua stack.
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    ByteBuffer *bb = ll_check_ByteBuffer(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    size_t nbytes = ll_check_size_t(_fun, L, 3);
    size_t nout = 0;
    if (bbufferWriteStream(bb, stream->f, nbytes, &nout))
        return ll_push_nil(_fun, L);
    ll_push_size_t(_fun, L, nout);
    return 1;
}

/**
 * \brief Check Lua stack at index %arg for user data of class ByteBuffer*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the ByteBuffer* contained in the user data.
 */
ByteBuffer *
ll_check_ByteBuffer(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<ByteBuffer>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the ByteBuffer* contained in the user data.
 */
ByteBuffer *
ll_opt_ByteBuffer(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_ByteBuffer(_fun, L, arg);
}

/**
 * \brief Push BMF user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param bb pointer to the ByteBuffer
 * \return 1 ByteBuffer* on the Lua stack.
 */
int
ll_push_ByteBuffer(const char *_fun, lua_State *L, ByteBuffer *bb)
{
    if (!bb)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, bb);
}

/**
 * \brief Create and push a new ByteBuffer*.
 * \param L Lua state.
 * \return 1 ByteBuffer* on the Lua stack.
 */
int
ll_new_ByteBuffer(lua_State *L)
{
    FUNC("ll_new_ByteBuffer");
    ByteBuffer *bbuffer = nullptr;
    size_t size = 0;
    const l_uint8 *indata = nullptr;
    l_int32 nbytes = 0;

    if (ll_isstring(_fun, L, 1)) {
        indata = ll_check_lbytes(_fun, L, 1, &size);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %" PRIu64 "\n", _fun,
            "indata", reinterpret_cast<const void *>(indata),
            "size", static_cast<l_intptr_t>(size));
        nbytes = static_cast<l_int32>(size);
        bbuffer = bbufferCreate(indata, nbytes);
    }

    if (!bbuffer) {
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %" PRIu64 "\n", _fun,
            "indata", reinterpret_cast<const void *>(indata),
            "size", static_cast<l_intptr_t>(size));
        bbuffer = bbufferCreate(indata, nbytes);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(bbuffer));
    return ll_push_ByteBuffer(_fun, L, bbuffer);
}

/**
 * \brief Register the BMF methods and functions in the TNAME meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_ByteBuffer(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_ByteBuffer},
        {"__tostring",          toString},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"DestroyAndSaveData",  DestroyAndSaveData},
        {"ExtendArray",         ExtendArray},
        {"Read",                Read},
        {"ReadStream",          ReadStream},
        {"Write",               Write},
        {"WriteStream",         WriteStream},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_ByteBuffer);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
