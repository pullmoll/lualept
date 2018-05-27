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
 *  Lua class PdfData
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_PDFDATA */
#define LL_FUNC(x) FUNC(LL_PDFDATA "." x)

/**
 * \brief Destroy a PdfData*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    PdfData **ppd = ll_check_udata<PdfData>(_fun, L, 1, LL_PDFDATA);
    PdfData *pd = *ppd;
    DBG(LOG_DESTROY, "%s: '%s' pcd=%p pd=%p\n",
        _fun, LL_PDFDATA, ppd, pd);
    ll_free(pd);
    *ppd = nullptr;
    return 0;
}

/**
 * \brief Create a new PdfData*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 PdfData* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    PdfData *pd = ll_calloc<PdfData>(_fun, L, 1);
    return ll_push_PdfData(_fun, L, pd);
}

/**
 * \brief Check Lua stack at index %arg for udata of class LL_PDFDATA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PdfData* contained in the user data
 */
PdfData *
ll_check_PdfData(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<PdfData>(_fun, L, arg, LL_PDFDATA);
}

/**
 * \brief Optionally expect a LL_DLLIST at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the PdfData* contained in the user data
 */
PdfData *
ll_check_PdfData_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_PdfData(_fun, L, arg);
}
/**
 * \brief Push BMF user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_PdfData
 * \return 1 PdfData* on the Lua stack
 */
int
ll_push_PdfData(const char *_fun, lua_State *L, PdfData *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_PDFDATA, cd);
}
/**
 * \brief Create and push a new PdfData*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 PdfData* on the Lua stack
 */
int
ll_new_PdfData(lua_State *L)
{
    return Create(L);
}
/**
 * \brief Register the BMF methods and functions in the LL_PDFDATA meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_PdfData(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},
        {"Create",              Create},
        {"Destroy",             Destroy},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_PDFDATA);
    return ll_register_class(L, LL_PDFDATA, methods, functions);
}
