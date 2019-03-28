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
 * \file llccbord.cpp
 * \class CCBord
 *
 * A class for handling c.c. borders.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_CCBORD

/** Define a function's name (_fun) with prefix CCBord */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief toString.
 * \param L Lua state.
 * \return 1 string on the Lua stack.
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    CCBord *ccb = ll_check_CCBord(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!ccb) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, LL_STRBUFF,
                TNAME "*: %p",
                reinterpret_cast<void *>(ccb));
        luaL_addstring(&B, str);
#if defined(LUALEPT_INTERNALS) && (LUALEPT_INTERNALS > 0)
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "pix", LL_PIX, reinterpret_cast<void *>(ccb->pix));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "boxa", LL_BOXA, reinterpret_cast<void *>(ccb->boxa));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "start", LL_PTA, reinterpret_cast<void *>(ccb->start));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %d",
                 "refcount", ccb->refcount);
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "local", LL_PTAA, reinterpret_cast<void *>(ccb->local));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "global", LL_PTAA, reinterpret_cast<void *>(ccb->global));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "step", LL_NUMAA, reinterpret_cast<void *>(ccb->step));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "splocal", LL_PTA, reinterpret_cast<void *>(ccb->splocal));
        luaL_addstring(&B, str);
        snprintf(str, LL_STRBUFF, "\n    %-14s: %s* %p",
                 "spglobal", LL_PTA, reinterpret_cast<void *>(ccb->spglobal));
        luaL_addstring(&B, str);
#endif
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Destroy a CCBord* (%ccbord).
 *
 * \param L Lua state.
 * \return 0 for nothing on the Lua stack.
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    CCBord *ccb = ll_take_udata<CCBord>(_fun, L, 1, TNAME);
    DBG(LOG_DESTROY, "%s: '%s' %s = %p\n", _fun,
        TNAME,
        "ccb", reinterpret_cast<void *>(ccb));
    ccbDestroy(&ccb);
    return 0;
}

/**
 * \brief Create a new CCBord* (%ccbord).
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * </pre>
 * \param L Lua state.
 * \return 1 CCBord* on the Lua stack.
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    Pix* pixs = ll_check_Pix(_fun, L, 1);
    CCBord *ccbord = ccbCreate(pixs);
    return ll_push_CCBord(_fun, L, ccbord);
}

/**
 * \brief Get the outer border from Pix* (%pixs) for CCBord* (%ccb).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBord* (ccb).
 * Arg #2 is expected to be a Pix* (pixs).
 * Arg #3 is expected to be a Box* (box).
 *
 * Leptonica's Notes:
 *      (1) the border is saved in relative coordinates within
 *          the c.c. (pixs).  Because the calculation is done
 *          in pixb with added 1 pixel border, we must subtract
 *          1 from each pixel value before storing it.
 *      (2) the stopping condition is that after the first pixel is
 *          returned to, the next pixel is the second pixel.  Having
 *          these 2 pixels recur in sequence proves the path is closed,
 *          and we do not store the second pixel again.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack.
 */
static int
GetOuterBorder(lua_State *L)
{
    LL_FUNC("GetOuterBorder");
    CCBord *ccb = ll_check_CCBord(_fun, L, 1);
    Pix *pixs = ll_check_Pix(_fun, L, 2);
    Box *box = ll_check_Box(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == pixGetOuterBorder(ccb, pixs, box));
}

/**
 * \brief Check Lua stack at index (%arg) for user data of class CCBord*.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBord* contained in the user data.
 */
CCBord *
ll_check_CCBord(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<CCBord>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a CCBord* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBord* contained in the user data.
 */
CCBord *
ll_opt_CCBord(const char *_fun, lua_State *L, int arg)
{
    if (!ll_isudata(_fun, L, arg, TNAME))
        return nullptr;
    return ll_check_CCBord(_fun, L, arg);
}

/**
 * \brief Push CCBord* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L Lua state.
 * \param cd pointer to the L_CCBord
 * \return 1 CCBord* on the Lua stack.
 */
int
ll_push_CCBord(const char *_fun, lua_State *L, CCBord *cd)
{
    if (!cd)
        return ll_push_nil(_fun, L);
    return ll_push_udata(_fun, L, TNAME, cd);
}

/**
 * \brief Create and push a new CCBord*.
 * \param L Lua state.
 * \return 1 CCBord* on the Lua stack.
 */
int
ll_new_CCBord(lua_State *L)
{
    FUNC("ll_new_CCBord");
    CCBord *ccb = nullptr;

    if (ll_isudata(_fun, L, 1, LL_PIX)) {
        Pix* pixs = ll_opt_Pix(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pixs));
        ccb = ccbCreate(pixs);
    }

    if (!ccb) {
        /* FIXME: create data for no pix? */
        Pix *pixs = nullptr;
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pixs));
        ccb = ccbCreate(pixs);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(ccb));
    return ll_push_CCBord(_fun, L, ccb);
}

/**
 * \brief Register the CCBord methods and functions in the CCBord meta table.
 * \param L Lua state.
 * \return 1 table on the Lua stack.
 */
int
ll_open_CCBord(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_CCBord},
        {"__tostring",          toString},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"GetOuterBorder",      GetOuterBorder},
        LUA_SENTINEL
    };
    LO_FUNC(TNAME);
    ll_set_global_cfunct(_fun, L, TNAME, ll_new_CCBord);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
