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
 * \file llpixacc.cpp
 * \class Pixacc
 *
 * A pixels accumulator with 32 bits per pixel.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_PIXACC

/** Define a function's name (_fun) with prefix Pixacc */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Pixacc* (%pixacc).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* (pixacc).
 *
 * Leptonica's Notes:
 *      (1) Always nulls the input ptr.
 * </pre>
 * \param L Lua state.
 * \return 1 void on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Pixacc *pixacc = ll_take_udata<Pixacc>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %d\n", _fun,
        TNAME,
        "pixacc", reinterpret_cast<void *>(pixacc),
        "offset", pixaccGetOffset(pixacc));
    pixaccDestroy(&pixacc);
    return 0;
}

/**
 * \brief Printable string for a Pixacc* (%queue).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* user data.
 * </pre>
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    Pixacc *pixacc = ll_check_Pixacc(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!pixacc) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                 TNAME "*: %p",
                 reinterpret_cast<void *>(pixacc));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "w", pixacc->w);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "h", pixacc->h);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %d",
                 "offset", pixacc->offset);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF,
                 "\n    %-14s: %s* %p",
                 "pix", LL_PIX, reinterpret_cast<void *>(pixacc->pix));
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}


/**
 * \brief Add a Pix* (%pix) to the Pixacc* (%pixacc).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* (pixacc).
 * Arg #2 is expected to be a Pix* (pix).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
Add(lua_State *L)
{
    LL_FUNC("Add");
    Pixacc *pixacc = ll_check_Pixacc(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_ok ok = pixaccAdd(pixacc, pix);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Create a Pixacc* (%pixacc) with width (%w) and height (%h).
 * <pre>
 * Arg #1 is expected to be a l_int32 (w).
 * Arg #2 is expected to be a l_int32 (h).
 * Arg #3 is expected to be a boolean (negflag).
 *
 * Leptonica's Notes:
 *      (1) Use %negflag = 1 for safety if any negative numbers are going
 *          to be used in the chain of operations.  Negative numbers
 *          arise, e.g., by subtracting a pix, or by adding a pix
 *          that has been pre-multiplied by a negative number.
 *      (2) Initializes the internal 32 bpp pix, similarly to the
 *          initialization in pixInitAccumulate().
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixacc* (%pixacc) on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 w = ll_check_l_int32(_fun, L, 1);
    l_int32 h = ll_check_l_int32(_fun, L, 2);
    l_int32 negflag = ll_opt_boolean(_fun, L, 3, TRUE);
    Pixacc *pixacc = pixaccCreate(w, h, negflag);
    return ll_push_Pixacc(_fun, L, pixacc);
}

/**
 * \brief CreateFromPix() brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a boolean (negflag).
 *
 * Leptonica's Notes:
 *      (1) See pixaccCreate()
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pixacc* (%pixacc) on the Lua stack
 */
static int
CreateFromPix(lua_State *L)
{
    LL_FUNC("CreateFromPix");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 negflag = ll_check_l_int32(_fun, L, 2);
    Pixacc *pixacc = pixaccCreateFromPix(pix, negflag);
    return ll_push_Pixacc(_fun, L, pixacc);
}

/**
 * \brief Get the final Pix* (%pix) with depth (%outdepth) for Pixacc* (%pixacc).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* (pixacc).
 * Arg #2 is expected to be a l_int32 (outdepth).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* (%pix) on the Lua stack
 */
static int
Final(lua_State *L)
{
    LL_FUNC("Final");
    Pixacc *pixacc = ll_check_Pixacc(_fun, L, 1);
    l_int32 outdepth = ll_check_l_int32(_fun, L, 2);
    Pix *pix = pixaccFinal(pixacc, outdepth);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get the offset of Pixacc* (%pixacc).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* (pixacc).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer (%offset) on the Lua stack
 */
static int
GetOffset(lua_State *L)
{
    LL_FUNC("GetOffset");
    Pixacc *pixacc = ll_check_Pixacc(_fun, L, 1);
    l_int32 offset = pixaccGetOffset(pixacc);
    return ll_push_l_int32(_fun, L, offset);
}

/**
 * \brief Get the Pix* (%pix) from the Pixacc* (%pixacc).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* (pixacc).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
GetPix(lua_State *L)
{
    LL_FUNC("GetPix");
    Pixacc *pixacc = ll_check_Pixacc(_fun, L, 1);
    Pix *pix = pixaccGetPix(pixacc);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Multiply Pixacc* (%pixacc) with a constant (%factor).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* (pixacc).
 * Arg #2 is expected to be a l_float32 (factor).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
MultConst(lua_State *L)
{
    LL_FUNC("MultConst");
    Pixacc *pixacc = ll_check_Pixacc(_fun, L, 1);
    l_float32 factor = ll_check_l_float32(_fun, L, 2);
    l_ok ok = pixaccMultConst(pixacc, factor);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Multiply Pix* (%pix) with a constant (%factor) and accumulate in Pixacc* (%pixacc).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* (pixacc).
 * Arg #2 is expected to be a Pix* (pix).
 * Arg #3 is expected to be a l_float32 (factor).
 *
 * Leptonica's Notes:
 *      (1) This creates a temp pix that is %pix multiplied by the
 *          constant %factor.  It then adds that into %pixacc.
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
MultConstAccumulate(lua_State *L)
{
    LL_FUNC("MultConstAccumulate");
    Pixacc *pixacc = ll_check_Pixacc(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_float32 factor = ll_check_l_float32(_fun, L, 3);
    l_ok ok = pixaccMultConstAccumulate(pixacc, pix, factor);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Subtract Pix* (%pix) from Pixacc* (%pixacc).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pixacc* (pixacc).
 * Arg #2 is expected to be a Pix* (pix).
 *
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 on the Lua stack
 */
static int
Subtract(lua_State *L)
{
    LL_FUNC("Subtract");
    Pixacc *pixacc = ll_check_Pixacc(_fun, L, 1);
    Pix *pix = ll_check_Pix(_fun, L, 2);
    l_ok ok = pixaccSubtract(pixacc, pix);
    return ll_push_boolean(_fun, L, 0 == ok);
}

/**
 * \brief Check Lua stack at index (%arg) for user data of class Pixacc*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pixacc* contained in the user data.
 */
Pixacc *
ll_check_Pixacc(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Pixacc>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Pixacc* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Pixacc* contained in the user data.
 */
Pixacc *
ll_opt_Pixacc(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_Pixacc(_fun, L, arg);
}

/**
 * \brief Push Pixacc* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param cd pointer to the L_Pixacc
 * \return 1 Pixacc* on the Lua stack.
 */
int
ll_push_Pixacc(const char *_fun, lua_State *L, Pixacc *pixacc)
{
    if (!pixacc)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, pixacc);
}

/**
 * \brief Create and push a new Pixacc*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L Lua state.
 * \return 1 Pixacc* on the Lua stack.
 */
int
ll_new_Pixacc(lua_State *L)
{
    FUNC("ll_new_Pixacc");
    Pixacc *pixacc = nullptr;
    Pix* pix = nullptr;
    l_int32 width = 1;
    l_int32 height = 1;
    l_int32 negflag = TRUE;

    if (ll_isudata(_fun, L, 1, LL_PIX)) {
        pix = ll_check_Pix(_fun, L, 1);
        negflag = ll_opt_boolean(_fun, L, 2, TRUE);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %s\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pix),
            "negflag", negflag ? "TRUE" : "FALSE");
        pixacc = pixaccCreateFromPix(pix, negflag);
    }

    if (!pixacc && ll_isinteger(_fun, L, 1)) {
        width = ll_opt_l_int32(_fun, L, 1, width);
        height = ll_opt_l_int32(_fun, L, 2, height);
        negflag = ll_opt_boolean(_fun, L, 3, TRUE);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = %s\n", _fun,
            "width", width,
            "height", height,
            "negflag", negflag ? "TRUE" : "FALSE");
        pixacc = pixaccCreate(width, height, negflag);
    }

    if (!pixacc) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = %s\n", _fun,
            "width", width,
            "height", height,
            "negflag", negflag ? "TRUE" : "FALSE");
        pixacc = pixaccCreate(width, height, negflag);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(pixacc));
    return ll_push_Pixacc(_fun, L, pixacc);
}

/**
 * \brief Register the Pixacc methods and functions in the Pixacc meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_Pixacc(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_Pixacc},
        {"__tostring",          toString},
        {"Add",                 Add},
        {"Create",              Create},
        {"CreateFromPix",       CreateFromPix},
        {"Destroy",             Destroy},
        {"Final",               Final},
        {"GetOffset",           GetOffset},
        {"GetPix",              GetPix},
        {"MultConst",           MultConst},
        {"MultConstAccumulate", MultConstAccumulate},
        {"Subtract",            Subtract},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_Pixacc);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
