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
 *  Lua class Pixaa
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_PIXAA */
#define LL_FUNC(x) FUNC(LL_PIXAA "." x)

/**
 * \brief Destroy a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Pixaa **ppixaa = ll_check_udata<Pixaa>(_fun, L, 1, LL_PIXAA);
    Pixaa *pixaa = *ppixaa;
    DBG(LOG_DESTROY, "%s: '%s' ppixaa=%p pixaa=%p count=%d\n",
        _fun, LL_PIXAA, ppixaa, pixaa, pixaaGetCount(pixaa, nullptr));
    pixaaDestroy(&pixaa);
    *ppixaa = nullptr;
    return 0;
}

/**
 * \brief Create a new Pixaa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixaa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    Pixaa *pixaa = pixaaCreate(n);
    return ll_push_Pixaa(_fun, L, pixaa);
}

/**
 * \brief Get count for a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 integer (count) and Numa* (na) the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    Numa *na = nullptr;
    lua_pushinteger(L, pixaaGetCount(pixaa, &na));
    return 1 + ll_push_Numa(_fun, L, na);
}

/**
 * \brief Add a Box* to a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is expected to be a Box* user data (box).
 * Arg #3 is optionally a string defining the copyflag.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddBox(lua_State *L)
{
    LL_FUNC("AddBox");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    Box *box = ll_check_Box(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == pixaaAddBox(pixaa, box, copyflag));
}

/**
 * \brief Add a Pix* and its Box* to a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix* user data (pix).
 * Arg #4 is expected to be a Box* user data (box).
 * Arg #5 is optionally a string defining the copyflag.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPix(lua_State *L)
{
    LL_FUNC("AddPix");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaaGetCount(pixaa, nullptr));
    Pix *pix = ll_check_Pix(_fun, L, 3);
    Box *box = ll_check_Box(_fun, L, 4);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 5, L_COPY);
    return ll_push_boolean(_fun, L, 0 == pixaaAddPix(pixaa, idx, pix, box, copyflag));
}

/**
 * \brief Add a Pixa* to a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is expected to be a Pixa* user data.
 * Arg #3 is optionally a string defining the copyflag.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddPixa(lua_State *L)
{
    LL_FUNC("AddPixa");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    Pixa *pixa = ll_check_Pixa(_fun, L, 2);
    l_int32 flag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == pixaaAddPixa(pixaa, pixa, flag));
}

/**
 * \brief Clear the Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Clear(lua_State *L)
{
    LL_FUNC("Clear");
    Pixaa *paa = ll_check_Pixaa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixaaClear(paa));
}

/**
 * \brief Create a new Pixaa* from a Pixa*.
 * <pre>
 * Arg #1 is expected to be a Pixa* user data.
 * Arg #2 is expected to be a l_int32 (n).
 * Arg #3 is optional and, if given, expected to be a string (type).
 * Arg #4 is optional and, if given, expected to be a string (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixaa* on the Lua stack
 */
static int
CreateFromPixa(lua_State *L)
{
    LL_FUNC("CreateFromPixa");
    Pixa *pixa = ll_check_Pixa(_fun, L, 1);
    l_int32 n = ll_check_l_int32_default(_fun, L, 2, 1);
    l_int32 type = ll_check_consecutive_skip_by(_fun, L, 3, L_CHOOSE_CONSECUTIVE);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 4, L_CLONE);
    Pixaa *pixaa = pixaaCreateFromPixa(pixa, n, type, copyflag);
    return ll_push_Pixaa(_fun, L, pixaa);
}

/**
 * \brief Extend array of a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ExtendArray(lua_State *L)
{
    LL_FUNC("ExtendArray");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixaaExtendArray(pixaa));
}

/**
 * \brief Get a Boxa* from a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is optionally a string defining the access flag (copy, clone).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1: Boxa* on the Lua stack , or 0 on error
 */
static int
GetBoxa(lua_State *L)
{
    LL_FUNC("GetBoxa");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    l_int32 accesstype = ll_check_access_storage(_fun, L, 2, L_CLONE);
    Boxa *boxa = pixaaGetBoxa(pixaa, accesstype);
    return ll_push_Boxa(_fun, L, boxa);
}

/**
 * \brief Get a Pixa* from a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is optionally a string defining the access flag (copy, clone).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1: Pixa* on the Lua stack , or 0 on error
 */
static int
GetPixa(lua_State *L)
{
    LL_FUNC("GetPixa");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaaGetCount(pixaa, nullptr));
    l_int32 accesstype = ll_check_access_storage(_fun, L, 3, L_CLONE);
    Pixa *pixa = pixaaGetPixa(pixaa, idx, accesstype);
    return ll_push_Pixa(_fun, L, pixa);
}

/**
 * \brief Join the Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is expected to be another Pixaa* user data.
 * Arg #3 is optional and expected to be a l_int32 (istart).
 * Arg #4 is optional and expected to be a l_int32 (iend).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Join(lua_State *L)
{
    LL_FUNC("Join");
    Pixaa *pixaad = ll_check_Pixaa(_fun, L, 1);
    Pixaa *pixaas = ll_check_Pixaa(_fun, L, 2);
    l_int32 istart = ll_check_l_int32_default(_fun, L, 3, 1) - 1;
    l_int32 iend = ll_check_l_int32_default(_fun, L, 3, pixaaGetCount(pixaas, nullptr)) - 1;
    return ll_push_boolean(_fun, L, 0 == pixaaJoin(pixaad, pixaas, istart, iend));
}

/**
 * \brief Read a Pixaa* from an external file.
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixaa* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Pixaa *pixaa = pixaaRead(filename);
    return ll_push_Pixaa(_fun, L, pixaa);
}

/**
 * \brief Read a Pixaa* (%pixaa) from a number of external files.
 * <pre>
 * Arg #1 is expected to be a string containing the directory (dirname).
 * Arg #2 is expected to be a string (substr).
 * Arg #3 is expected to be a l_int32 (first)
 * Arg #4 is expected to be a l_int32 (nfiles)
 *
 * Note:
 * If %substr is ommited, %first and %nfiles are expected as #2 and #3.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixaa* on the Lua stack
 */
static int
ReadFromFiles(lua_State *L)
{
    LL_FUNC("ReadFromFiles");
    const char *dirname = ll_check_string(_fun, L, 1);
    const char *substr = nullptr;
    l_int32 first = 0;
    l_int32 nfiles = 0;
    Pixaa *pixaa = nullptr;
    if (lua_isinteger(L, 2) && lua_isinteger(L, 3)) {
        first = ll_check_l_int32_default(_fun, L, 2, 0);
        nfiles = ll_check_l_int32_default(_fun, L, 3, 0);
    } else {
        substr = ll_check_string(_fun, L, 2);
        first = ll_check_l_int32_default(_fun, L, 3, 0);
        nfiles = ll_check_l_int32_default(_fun, L, 4, 0);
    }
    pixaa = pixaaReadFromFiles(dirname, substr, first, nfiles);
    return ll_push_Pixaa(_fun, L, pixaa);
}

/**
 * \brief Read a Pixaa* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixaa* on the Lua stack
 */
static int
ReadMem(lua_State *L)
{
    LL_FUNC("ReadMem");
    size_t len;
    const char *data = ll_check_lstring(_fun, L, 1, &len);
    Pixaa *pixaa = pixaaReadMem(reinterpret_cast<const l_uint8 *>(data), len);
    return ll_push_Pixaa(_fun, L, pixaa);
}

/**
 * \brief Read a Pixaa* from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixaa* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Pixaa *pixaa = pixaaReadStream(stream->f);
    return ll_push_Pixaa(_fun, L, pixaa);
}

/**
 * \brief Replace the Pixa* in a Pixaa* at index %idx.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a Pix* user data (pixa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ReplacePixa(lua_State *L)
{
    LL_FUNC("ReplacePixa");
    Pixaa *paa = ll_check_Pixaa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, pixaaGetCount(paa, nullptr));
    Pixa *pa = ll_check_Pixa(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixaaReplacePixa(paa, idx, pa));
}

/**
 * \brief Truncate array of a Pixaa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Truncate(lua_State *L)
{
    LL_FUNC("Truncate");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == pixaaTruncate(pixaa));
}

/**
 * \brief Write the Pixaa* (%pixaa) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is expected to be string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixaaWrite(filename, pixaa));
}

/**
 * \brief Write the Pixaa* (%pixaa) to memory and return it as a Lua string.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteMem(lua_State *L)
{
    LL_FUNC("WriteMem");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    l_uint8 *data = nullptr;
    size_t size = 0;
    if (pixaaWriteMem(&data, &size, pixaa))
        return ll_push_nil(L);
    lua_pushlstring(L, reinterpret_cast<const char *>(data), size);
    LEPT_FREE(data);
    return 1;
}

/**
 * \brief Write the Pixaa* to an external file.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixaa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Pixaa *pixaa = ll_check_Pixaa(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == pixaaWriteStream(stream->f, pixaa));
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_PIXAA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pixaa* contained in the user data
 */
Pixaa *
ll_check_Pixaa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Pixaa>(_fun, L, arg, LL_PIXAA);
}

/**
 * \brief Optionally expect a LL_PIXAA at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pixaa* contained in the user data
 */
Pixaa *
ll_check_Pixaa_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Pixaa(_fun, L, arg);
}

/**
 * \brief Push Pixaa* user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param pixaa pointer to the PIXAA
 * \return 1 Pixaa* on the Lua stack
 */
int
ll_push_Pixaa(const char *_fun, lua_State *L, Pixaa *pixaa)
{
    if (!pixaa)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_PIXAA, pixaa);
}
/**
 * \brief Create a new Pixaa*.
 * \param L pointer to the lua_State
 * \return 1 Pixaa* on the Lua stack
 */
int
ll_new_Pixaa(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the PIX methods and functions in the LL_PIX meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Pixaa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               Create},
        {"__len",               GetCount},
        {"AddBox",              AddBox},
        {"AddPix",              AddPix},
        {"AddPixa",		AddPixa},
        {"Clear",               Clear},
        {"Create",              Create},
        {"CreateFromPixa",      CreateFromPixa},
        {"Destroy",             Destroy},
        {"ExtendArray",		ExtendArray},
        {"GetBoxa",             GetBoxa},
        {"GetCount",            GetCount},
        {"GetPixa",             GetPixa},
        {"Join",                Join},
        {"Read",                Read},
        {"ReadFromFiles",       ReadFromFiles},
        {"ReadMem",             ReadMem},
        {"ReadStream",          ReadStream},
        {"ReplacePixa",		ReplacePixa},
        {"Truncate",		Truncate},
        {"Write",               Write},
        {"WriteMem",            WriteMem},
        {"WriteStream",         WriteStream},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_PIXAA);
    return ll_register_class(L, LL_PIXAA, methods, functions);
}
