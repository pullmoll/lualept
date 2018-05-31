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
 *  Lua class CCBorda
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_CCBORDA */
#define LL_FUNC(x) FUNC(LL_CCBORDA "." x)



/**
 * \brief Destroy a CCBorda*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    CCBorda **pccba = ll_check_udata<CCBorda>(_fun, L, 1, LL_CCBORDA);
    CCBorda *ccba = *pccba;
    DBG(LOG_DESTROY, "%s: '%s' pccba=%p ccba=%p\n",
        _fun, LL_CCBORDA, pccba, ccba);
    ccbaDestroy(&ccba);
    *pccba = nullptr;
    return 0;
}

/**
 * \brief Create a new CCBorda*.
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CCBorda* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    Pix* pixs = ll_check_Pix(_fun, L, 1);
    l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
    CCBorda *ccba = ccbaCreate(pixs, n);
    return ll_push_CCBorda(_fun, L, ccba);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    return ll_push_l_int32(_fun, L, ccbaGetCount(ccba));
}

/**
 * \brief Add the CCBord* (%ccb) to the CCBorda* (%ccba).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a CCBord* (ccb).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddCcb(lua_State *L)
{
    LL_FUNC("AddCcb");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    CCBord *ccb = ll_check_CCBord(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaAddCcb(ccba, ccb));
}

/**
 * \brief Display the CCBora* (%ccba) in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
DisplayBorder(lua_State *L)
{
    LL_FUNC("DisplayBorder");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    Pix *pix = ccbaDisplayBorder(ccba);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
DisplayImage1(lua_State *L)
{
    LL_FUNC("DisplayImage1");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    Pix *pix = ccbaDisplayImage1(ccba);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
DisplayImage2(lua_State *L)
{
    LL_FUNC("DisplayImage2");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    Pix *pix = ccbaDisplayImage2(ccba);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
DisplaySPBorder(lua_State *L)
{
    LL_FUNC("DisplaySPBorder");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    Pix *pix = ccbaDisplaySPBorder(ccba);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GenerateGlobalLocs(lua_State *L)
{
    LL_FUNC("GenerateGlobalLocs");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == ccbaGenerateGlobalLocs(ccba));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a l_int32 (ptsflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GenerateSPGlobalLocs(lua_State *L)
{
    LL_FUNC("GenerateSPGlobalLocs");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    l_int32 ptsflag = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaGenerateSPGlobalLocs(ccba, ptsflag));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a l_int32 (index).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CCBord* on the Lua stack
 */
static int
GetCcb(lua_State *L)
{
    LL_FUNC("GetCcb");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    l_int32 index = ll_check_index(_fun, L, 2, ccbaGetCount(ccba));
    CCBord *ccb = ccbaGetCcb(ccba, index);
    return ll_push_CCBord(_fun, L, ccb);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CCBorda* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    CCBorda *result = ccbaRead(filename);
    return ll_push_CCBorda(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CCBorda* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    CCBorda *result = ccbaReadStream(stream->f);
    return ll_push_CCBorda(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a l_int32 (coordtype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
StepChainsToPixCoords(lua_State *L)
{
    LL_FUNC("StepChainsToPixCoords");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    l_int32 coordtype = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaStepChainsToPixCoords(ccba, coordtype));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaWrite(filename, ccba));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteSVG(lua_State *L)
{
    LL_FUNC("WriteSVG");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == ccbaWriteSVG(filename, ccba));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
WriteSVGString(lua_State *L)
{
    LL_FUNC("WriteSVGString");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 2);
    const char *filename = ll_check_string(_fun, L, 1);
    char *result = ccbaWriteSVGString(filename, ccba);
    return ll_push_string(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    l_int32 result = ccbaWriteStream(stream->f, ccba);
    return ll_push_l_int32(_fun, L, result);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GenerateSinglePath(lua_State *L)
{
    LL_FUNC("GenerateSinglePath");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == ccbaGenerateSinglePath(ccba));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
GenerateStepChains(lua_State *L)
{
    LL_FUNC("GenerateStepChains");
    CCBorda *ccba = ll_check_CCBorda(_fun, L, 1);
    return ll_push_boolean(_fun, L, 0 == ccbaGenerateStepChains(ccba));
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class LL_CCBORDA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBorda* contained in the user data
 */
CCBorda *
ll_check_CCBorda(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<CCBorda>(_fun, L, arg, LL_CCBORDA);
}

/**
 * \brief Optionally expect a CCBorda* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBorda* contained in the user data
 */
CCBorda *
ll_opt_CCBorda(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_CCBorda(_fun, L, arg);
}

/**
 * \brief Push CCBorda* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_CCBorda
 * \return 1 CCBorda* on the Lua stack
 */
int
ll_push_CCBorda(const char *_fun, lua_State *L, CCBorda *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_CCBORDA, cd);
}

/**
 * \brief Create and push a new CCBorda*.
 * \param L pointer to the lua_State
 * \return 1 CCBorda* on the Lua stack
 */
int
ll_new_CCBorda(lua_State *L)
{
    FUNC("ll_new_CCBorda");
    CCBorda *ccba = nullptr;

    if (lua_isuserdata(L, 1)) {
        Pix* pixs = ll_opt_Pix(_fun, L, 1);
        if (pixs) {
            l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
            DBG(LOG_NEW_CLASS, "%s: create for %s* = %p, %s = %n\n", _fun,
                LL_PIX, reinterpret_cast<void *>(pixs), "n", n);
            ccba = ccbaCreate(pixs, n);
        } else {
            luaL_Stream *stream = ll_check_stream(_fun, L, 1);
            DBG(LOG_NEW_CLASS, "%s: create for %s* = %p\n", _fun,
                LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
            ccba = ccbaReadStream(stream->f);
        }
    }

    if (!ccba && lua_isstring(L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_CLASS, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        ccba = ccbaRead(filename);
    }

    if (!ccba) {
        /* FIXME: create data for no pix? */
        DBG(LOG_NEW_CLASS, "%s: create for %s* = %p, %s = %d\n", _fun,
            LL_PIX, nullptr, "n", 1);
        ccba = ccbaCreate(nullptr, 1);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        LL_CCBORDA, reinterpret_cast<void *>(ccba));
    return ll_push_CCBorda(_fun, L, ccba);
}

/**
 * \brief Register the CCBorda methods and functions in the LL_CCBORDA meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_CCBorda(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},           /* garbage collector */
        {"__new",                   ll_new_CCBorda},    /* CCBorda(pix,n) */
        {"__len",                   GetCount},
        {"AddCcb",                  AddCcb},
        {"Create",                  Create},
        {"Destroy",                 Destroy},
        {"DisplayBorder",           DisplayBorder},
        {"DisplayImage1",           DisplayImage1},
        {"DisplayImage2",           DisplayImage2},
        {"DisplaySPBorder",         DisplaySPBorder},
        {"GenerateGlobalLocs",      GenerateGlobalLocs},
        {"GenerateSPGlobalLocs",    GenerateSPGlobalLocs},
        {"GenerateSinglePath",      GenerateSinglePath},
        {"GenerateStepChains",      GenerateStepChains},
        {"GetCcb",                  GetCcb},
        {"GetCount",                GetCount},
        {"Read",                    Read},
        {"ReadStream",              ReadStream},
        {"StepChainsToPixCoords",   StepChainsToPixCoords},
        {"Write",                   Write},
        {"WriteSVG",                WriteSVG},
        {"WriteSVGString",          WriteSVGString},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, ll_new_CCBorda);
    lua_setglobal(L, LL_CCBORDA);
    return ll_register_class(L, LL_CCBORDA, methods, functions);
}
