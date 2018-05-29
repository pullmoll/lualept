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
 *  Lua class Sela
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_SELA */
#define LL_FUNC(x) FUNC(LL_SELA "." x)

/**
 * \brief Destroy a Sela*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Sela **psela = ll_check_udata<Sela>(_fun, L, 1, LL_SELA);
    Sela *sela = *psela;
    DBG(LOG_DESTROY, "%s: '%s' psela=%p sela=%p count=%d\n",
        _fun, LL_SELA, psela, sela, selaGetCount(sela));
    selaDestroy(&sela);
    *psela = nullptr;
    return 0;
}

/**
 * \brief Create a new Sela*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_check_l_int32_default(_fun, L, 1, 1);
    Sela *pa = selaCreate(n);
    return ll_push_Sela(_fun, L, pa);
}

/**
 * \brief Get then number Sel* in the Sela* (%sela).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    ll_push_l_int32(_fun, L, selaGetCount(sela));
    return 1;
}

/**
 * \brief Printable string for a Sela*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* user data.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Sela *sela = ll_check_Sela(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!sela) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 LL_SELA ": %p\n",
                 reinterpret_cast<void *>(sela));
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (selas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
static int
Thin4and8cc(lua_State *L)
{
    LL_FUNC("Thin4and8cc");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    Sela *sela = sela4and8ccThin(selas);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
static int
Thin4cc(lua_State *L)
{
    LL_FUNC("Thin4cc");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    Sela *sela = sela4ccThin(selas);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
static int
Thin8cc(lua_State *L)
{
    LL_FUNC("Thin8cc");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    Sela *sela = sela8ccThin(selas);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
static int
AddBasic(lua_State *L)
{
    LL_FUNC("AddBasic");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    Sela *sela = selaAddBasic(selas);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (selas).
 * Arg #2 is expected to be a l_float32 (hlsize).
 * Arg #3 is expected to be a l_float32 (mdist).
 * Arg #4 is expected to be a l_int32 (norient).
 * Arg #5 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
static int
AddCrossJunctions(lua_State *L)
{
    LL_FUNC("AddCrossJunctions");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    l_float32 hlsize = ll_check_l_float32(_fun, L, 2);
    l_float32 mdist = ll_check_l_float32(_fun, L, 3);
    l_int32 norient = ll_check_l_int32(_fun, L, 4);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 5);
    Sela *sela = selaAddCrossJunctions(selas, hlsize, mdist, norient, debugflag);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (selas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela * on the Lua stack
 */
static int
AddDwaCombs(lua_State *L)
{
    LL_FUNC("AddDwaCombs");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    Sela *sela = selaAddDwaCombs(selas);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela * on the Lua stack
 */
static int
AddDwaLinear(lua_State *L)
{
    LL_FUNC("AddDwaLinear");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    Sela *sela = selaAddDwaLinear(selas);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela * on the Lua stack
 */
static int
AddHitMiss(lua_State *L)
{
    LL_FUNC("AddHitMiss");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    Sela *sela = selaAddHitMiss(selas);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * Arg #2 is expected to be a l_float32 (hlsize).
 * Arg #3 is expected to be a l_float32 (mdist).
 * Arg #4 is expected to be a l_int32 (norient).
 * Arg #5 is expected to be a l_int32 (debugflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela * on the Lua stack
 */
static int
AddTJunctions(lua_State *L)
{
    LL_FUNC("AddTJunctions");
    Sela *selas = ll_check_Sela(_fun, L, 1);
    l_float32 hlsize = ll_check_l_float32(_fun, L, 2);
    l_float32 mdist = ll_check_l_float32(_fun, L, 3);
    l_int32 norient = ll_check_l_int32(_fun, L, 4);
    l_int32 debugflag = ll_check_l_int32(_fun, L, 5);
    Sela *sela = selaAddTJunctions(selas, hlsize, mdist, norient, debugflag);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * Arg #2 is expected to be a Sel* (sel).
 * Arg #3 is expected to be a string (selname).
 * Arg #4 is expected to be a l_int32 (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
AddSel(lua_State *L)
{
    LL_FUNC("AddSel");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    Sel *sel = ll_check_Sel(_fun, L, 2);
    const char *selname = ll_check_string(_fun, L, 3);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 4, L_COPY);
    return ll_push_boolean(_fun, L, 0 == selaAddSel(sela, sel, selname, copyflag));
}

/**
 * \brief Create a new Sela* (%sela) from an external file (%filename)
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela * on the Lua stack
 */
static int
CreateFromFile(lua_State *L)
{
    LL_FUNC("CreateFromFile");
    const char *filename = ll_check_string(_fun, L, 1);
    Sela *sela = selaCreateFromFile(filename);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * Arg #2 is expected to be a l_int32 (size).
 * Arg #3 is expected to be a l_int32 (gthick).
 * Arg #4 is expected to be a l_int32 (spacing).
 * Arg #5 is expected to be a l_int32 (ncols).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayInPix(lua_State *L)
{
    LL_FUNC("DisplayInPix");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    l_int32 size = ll_check_l_int32(_fun, L, 2);
    l_int32 gthick = ll_check_l_int32(_fun, L, 3);
    l_int32 spacing = ll_check_l_int32(_fun, L, 4);
    l_int32 ncols = ll_check_l_int32(_fun, L, 5);
    Pix *pix = selaDisplayInPix(sela, size, gthick, spacing, ncols);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * Arg #2 is expected to be a string (name).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 integer (%idx) and Sel* (%sel) on the Lua stack
 */
static int
FindSelByName(lua_State *L)
{
    LL_FUNC("FindSelByName");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    const char *name = ll_check_string(_fun, L, 2);
    l_int32 idx = 0;
    Sel *sel = nullptr;
    if (selaFindSelByName(sela, name, &idx, &sel))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, idx + 1);
    ll_push_Sel(_fun, L, sel);
    return 2;
}

/**
 * \brief Get Sel* with index (%idx) from Sela* (%sela).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * Arg #2 is expected to be a l_int32 (i).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
GetSel(lua_State *L)
{
    LL_FUNC("GetSel");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, selaGetCount(sela));
    return ll_push_Sel(_fun, L, selaGetSel(sela, idx));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sarray * on the Lua stack
 */
static int
GetSelnames(lua_State *L)
{
    LL_FUNC("GetSelnames");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    Sarray *sa = selaGetSelnames(sela);
    ll_push_Sarray(_fun, L, sa);
    sarrayDestroy(&sa);
    return 1;
}

/**
 * \brief Read a Sela* from an external file.
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Selaa* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Sela *sela = selaRead(filename);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Read a Sela* from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Selaa* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Sela *sela = selaReadStream(stream->f);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Write the Sela* (%sela) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Selaa* user data.
 * Arg #2 is expected to be string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == selaWrite(filename, sela));
}

/**
 * \brief Write the Sela* (%sela) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Selaa* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == selaWriteStream(stream->f, sela));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * Arg #2 is expected to be a l_int32 (hsize).
 * Arg #3 is expected to be a l_int32 (vsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetBrickName(lua_State *L)
{
    LL_FUNC("GetBrickName");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    l_int32 hsize = ll_check_l_int32(_fun, L, 2);
    l_int32 vsize = ll_check_l_int32(_fun, L, 3);
    char *name = selaGetBrickName(sela, hsize, vsize);
    lua_pushstring(L, name);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* (sela).
 * Arg #2 is expected to be a l_int32 (size).
 * Arg #3 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetCombName(lua_State *L)
{
    LL_FUNC("GetCombName");
    Sela *sela = ll_check_Sela(_fun, L, 1);
    l_int32 size = ll_check_l_int32(_fun, L, 2);
    l_int32 direction = ll_check_l_int32(_fun, L, 3);
    char *name = selaGetCombName(sela, size, direction);
    lua_pushstring(L, name);
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (index).
 * Arg #2 is expected to be a l_int32 (debug).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela * on the Lua stack
 */
static int
MakeThinSets(lua_State *L)
{
    LL_FUNC("MakeThinSets");
    l_int32 index = ll_check_l_int32(_fun, L, 1);
    l_int32 debug = ll_check_boolean_default(_fun, L, 2, FALSE);
    Sela *sela = selaMakeThinSets(index, debug);
    return ll_push_Sela(_fun, L, sela);
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_SELA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Sela* contained in the user data
 */
Sela *
ll_check_Sela(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Sela>(_fun, L, arg, LL_SELA);
}

/**
 * \brief Optionally expect a LL_SELA at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Sela* contained in the user data
 */
Sela *
ll_check_Sela_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Sela(_fun, L, arg);
}

/**
 * \brief Push PTA user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param sela pointer to the PTA
 * \return 1 Sela* on the Lua stack
 */
int
ll_push_Sela(const char *_fun, lua_State *L, Sela *sela)
{
    if (!sela)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_SELA, sela);
}
/**
 * \brief Create and push a new Sela*.
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
int
ll_new_Sela(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the PTA methods and functions in the LL_SELA meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Sela(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},       /* garbage collector */
        {"__new",               Create},        /* new Sela */
        {"__len",               GetCount},
        {"__tostring",          toString},
        {"Thin4and8cc",         Thin4and8cc},
        {"Thin4cc",             Thin4cc},
        {"Thin8cc",             Thin8cc},
        {"AddBasic",            AddBasic},
        {"AddCrossJunctions",   AddCrossJunctions},
        {"AddDwaCombs",         AddDwaCombs},
        {"AddDwaLinear",        AddDwaLinear},
        {"AddHitMiss",          AddHitMiss},
        {"AddTJunctions",       AddTJunctions},
        {"AddSel",              AddSel},
        {"Create",              Create},
        {"CreateFromFile",      CreateFromFile},
        {"Destroy",             Destroy},
        {"DisplayInPix",        DisplayInPix},
        {"FindSelByName",       FindSelByName},
        {"GetCount",            GetCount},
        {"GetSel",              GetSel},
        {"GetSelnames",         GetSelnames},
        {"Read",                Read},
        {"ReadStream",          ReadStream},
        {"Write",               Write},
        {"WriteStream",         WriteStream},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_SELA);
    return ll_register_class(L, LL_SELA, methods, functions);
}
