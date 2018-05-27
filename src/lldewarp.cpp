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
 *  Lua class Dewarp
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_DEWARP */
#define LL_FUNC(x) FUNC(LL_DEWARP "." x)

/**
 * \brief Destroy a Dewarp*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Dewarp **pdew = ll_check_udata<Dewarp>(_fun, L, 1, LL_DEWARP);
    Dewarp *dew = *pdew;
    DBG(LOG_DESTROY, "%s: '%s' pdew=%p dew=%p\n", _fun,
        LL_DEWARP, pdew, dew);
    dewarpDestroy(&dew);
    *pdew = nullptr;
    return 0;
}

/**
 * \brief Create a new Dewarp*.
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (pageno).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarp* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 pageno = ll_check_l_int32(_fun, L, 2);
    Dewarp *dew = dewarpCreate(pixs, pageno);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Printable string for a Dewarp*.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!dew) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 LL_DEWARP ": %p\n",
                 reinterpret_cast<void *>(dew));
        /* TODO: more info */
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Build line model for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a l_int32 (opensize).
 * Arg #3 is expected to be a const (char *debugfile).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 for l_int32 on the Lua stack
 */
static int
BuildLineModel(lua_State *L)
{
    LL_FUNC("BuildLineModel");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    l_int32 opensize = ll_check_l_int32(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dewarpBuildLineModel(dew, opensize, nullptr));
}

/**
 * \brief Build page model for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a L_DEWARP* (dew).
 * Arg #2 is expected to be a const (char *debugfile).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 for l_int32 on the Lua stack
 */
static int
BuildPageModel(lua_State *L)
{
    LL_FUNC("BuildPageModel");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    return ll_push_bool(_fun, L, 0 == dewarpBuildPageModel(dew, nullptr));
}

/**
 * \brief Create reference Dewarp* (%dew).
 * <pre>
 * Arg #1 is expected to be a l_int32 (pageno).
 * Arg #2 is expected to be a l_int32 (refpage).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarp* on the Lua stack
 */
static int
CreateRef(lua_State *L)
{
    LL_FUNC("CreateRef");
    l_int32 pageno = ll_check_l_int32(_fun, L, 1);
    l_int32 refpage = ll_check_l_int32(_fun, L, 2);
    Dewarp *dew = dewarpCreateRef(pageno, refpage);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Find horizontal disparity for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a Ptaa* (ptaa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FindHorizDisparity(lua_State *L)
{
    LL_FUNC("FindHorizDisparity");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 2);
    return ll_push_bool(_fun, L, dewarpFindHorizDisparity(dew, ptaa));
}

/**
 * \brief Find horizontal slope disparity for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a Pix* (pixb).
 * Arg #3 is expected to be a l_float32 (fractthresh).
 * Arg #4 is expected to be a l_int32 (parity).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FindHorizSlopeDisparity(lua_State *L)
{
    LL_FUNC("FindHorizSlopeDisparity");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    Pix *pixb = ll_check_Pix(_fun, L, 2);
    l_float32 fractthresh = ll_check_l_float32(_fun, L, 3);
    l_int32 parity = ll_check_l_int32(_fun, L, 4);
    return ll_push_bool(_fun, L, 0 == dewarpFindHorizSlopeDisparity(dew, pixb, fractthresh, parity));
}

/**
 * \brief Find vertical disparity for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a Ptaa* (ptaa).
 * Arg #3 is expected to be a l_int32 (rotflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
FindVertDisparity(lua_State *L)
{
    LL_FUNC("FindVertDisparity");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    Ptaa *ptaa = ll_check_Ptaa(_fun, L, 2);
    l_int32 rotflag = ll_check_l_int32(_fun, L, 3);
    return ll_push_bool(_fun, L, 0 == dewarpFindVertDisparity(dew, ptaa, rotflag));
}

/**
 * \brief Get text line centers for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Ptaa* on the Lua stack
 */
static int
GetTextlineCenters(lua_State *L)
{
    LL_FUNC("GetTextlineCenters");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 2);
    Ptaa *ptaa = dewarpGetTextlineCenters(pixs, debugflag);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Minimize Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Minimize(lua_State *L)
{
    LL_FUNC("Minimize");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    return ll_push_bool(_fun, L, 0 == dewarpMinimize(dew));
}

/**
 * \brief Populate full res for Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a Pix* (pix).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
PopulateFullRes(lua_State *L)
{
    LL_FUNC("PopulateFullRes");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    return ll_push_bool(_fun, L, 0 == dewarpPopulateFullRes(dew, pix, x, y));
}

/**
 * \brief Read Dewarp* (%dew) from external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarp* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Dewarp * dew = dewarpRead(filename);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Read Dewarp* (%dew) from a lstring (%str, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a const (l_uint8 *data).
 * Arg #2 is expected to be a size_t (size).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarp* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t size;
    const char *str = ll_check_lstring(_fun, L, 1, &size);
    l_uint8* data = reinterpret_cast<l_uint8 *>(reinterpret_cast<l_intptr_t>(str));
    Dewarp *dew = dewarpReadMem(data, size);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Read Dewarp* (%dew) from luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FILE* (fp).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Dewarp* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Dewarp *dew = dewarpReadStream(stream->f);
    return ll_push_Dewarp(_fun, L, dew);
}

/**
 * \brief Remove short lines from Dewarp* (%dew).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a Ptaa* (ptaas).
 * Arg #3 is expected to be a l_float32 (fract).
 * Arg #4 is expected to be a boolean (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Ptaa* on the Lua stack
 */
static int
RemoveShortLines(lua_State *L)
{
    LL_FUNC("RemoveShortLines");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    Ptaa *ptaas = ll_check_Ptaa(_fun, L, 2);
    l_float32 fract = ll_check_l_float32(_fun, L, 3);
    l_int32 debugflag = ll_check_boolean(_fun, L, 4);
    Ptaa * ptaa = dewarpRemoveShortLines(pixs, ptaas, fract, debugflag);
    return ll_push_Ptaa(_fun, L, ptaa);
}

/**
 * \brief Write Dewarp* (%dew) to external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 for l_int32 on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    ll_push_bool(_fun, L, 0 == dewarpWrite(filename, dew));
}

/**
 * \brief Write Dewarp* (%dew) to a lstring (%data, %size).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 for l_int32 on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (dewarpWriteMem(&data, &size, dew))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    ll_free(data);
    return 1;
}

/**
 * \brief Write Dewarp* (%dew) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Dewarp* (dew).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 for l_int32 on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Dewarp *dew = ll_check_Dewarp(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_bool(_fun, L, 0 == dewarpWriteStream(stream->f, dew));
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_DEWARP.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dewarp* contained in the user data
 */
Dewarp *
ll_check_Dewarp(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Dewarp>(_fun, L, arg, LL_DEWARP);
}

/**
 * \brief Optionally expect a LL_DEWARP at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Dewarp* contained in the user data
 */
Dewarp *
ll_check_Dewarp_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Dewarp(_fun, L, arg);
}
/**
 * \brief Push BOX user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param dew pointer to the Dewarp
 * \return 1 Dewarp* on the Lua stack
 */
int
ll_push_Dewarp(const char *_fun, lua_State *L, Dewarp *dew)
{
    if (!dew)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_DEWARP, dew);
}
/**
 * \brief Create and push a new Dewarp*.
 * \param L pointer to the lua_State
 * \return 1 Dewarp* on the Lua stack
 */
int
ll_new_Dewarp(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the Dewarp* methods and functions in the LL_DEWARP meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Dewarp(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},   /* garbage collector */
        {"__new",                   Create},    /* new Dewarp */
        {"__tostring",              toString},
        {"BuildLineModel",          BuildLineModel},
        {"BuildPageModel",          BuildPageModel},
        {"Create",                  Create},
        {"CreateRef",               CreateRef},
        {"Destroy",                 Destroy},
        {"FindHorizDisparity",      FindHorizDisparity},
        {"FindHorizSlopeDisparity", FindHorizSlopeDisparity},
        {"FindVertDisparity",       FindVertDisparity},
        {"GetTextlineCenters",      GetTextlineCenters},
        {"Minimize",                Minimize},
        {"PopulateFullRes",         PopulateFullRes},
        {"Read",                    Read},
        {"ReadMem",                 ReadMem},
        {"ReadStream",              ReadStream},
        {"RemoveShortLines",        RemoveShortLines},
        {"Write",                   Write},
        {"WriteMem",                WriteMem},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_DEWARP);
    return ll_register_class(L, LL_DEWARP, methods, functions);
}
