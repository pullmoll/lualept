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
 *  Lua class CCBord
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_CCBORD */
#define LL_FUNC(x) FUNC(LL_CCBORD "." x)

/**
 * \brief toString.
 * \param L pointer to the lua_State
 * @return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char str[256];
    CCBord *ccb = ll_check_CCBord(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!ccb) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                LL_CCBORD ": %p\n",
                reinterpret_cast<void *>(ccb));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    pix           : " LL_PIX "* %p\n", reinterpret_cast<void *>(ccb->pix));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    boxa          : " LL_BOXA "* %p\n", reinterpret_cast<void *>(ccb->boxa));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    start         : " LL_PTA "* %p\n", reinterpret_cast<void *>(ccb->start));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    refcount      : %d\n", ccb->refcount);
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    local         : " LL_PTAA "* %p\n", reinterpret_cast<void *>(ccb->local));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    global        : " LL_PTAA "* %p\n", reinterpret_cast<void *>(ccb->global));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    step          : " LL_NUMAA "* %p\n", reinterpret_cast<void *>(ccb->step));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    splocal       : " LL_PTA "* %p\n", reinterpret_cast<void *>(ccb->splocal));
        luaL_addstring(&B, str);
        snprintf(str, sizeof(str), "    spglobal      : " LL_PTA "* %p\n", reinterpret_cast<void *>(ccb->spglobal));
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Destroy a CCBord*.
 *
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    CCBord **pccbord = ll_check_udata<CCBord>(_fun, L, 1, LL_CCBORD);
    CCBord *ccbord = *pccbord;
    DBG(LOG_DESTROY, "%s: '%s' pccbord=%p ccbord=%p\n",
        _fun, LL_CCBORD, pccbord, ccbord);
    ccbDestroy(&ccbord);
    *pccbord = nullptr;
    return 0;
}

/**
 * \brief Create a new CCBord*.
 * <pre>
 * Arg #1 is expected to be a Pix* (pixs).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 CCBord* on the Lua stack
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
 * \brief Check Lua stack at index (%arg) for udata of class LL_CCBORD.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBord* contained in the user data
 */
CCBord *
ll_check_CCBord(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<CCBord>(_fun, L, arg, LL_CCBORD);
}

/**
 * \brief Optionally expect a CCBord* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBord* contained in the user data
 */
CCBord *
ll_opt_CCBord(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_CCBord(_fun, L, arg);
}
/**
 * \brief Push CCBord* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_CCBord
 * \return 1 CCBord* on the Lua stack
 */
int
ll_push_CCBord(const char *_fun, lua_State *L, CCBord *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_CCBORD, cd);
}
/**
 * \brief Create and push a new CCBord*.
 * \param L pointer to the lua_State
 * \return 1 CCBord* on the Lua stack
 */
int
ll_new_CCBord(lua_State *L)
{
    FUNC("ll_new_CCBord");
    CCBord *ccb = nullptr;

    if (lua_isuserdata(L, 1)) {
        Pix* pixs = ll_opt_Pix(_fun, L, 1);
        if (pixs) {
            DBG(LOG_NEW_CLASS, "%s: create for %s* = %p\n", _fun,
                LL_PIX, reinterpret_cast<void *>(pixs));
            ccb = ccbCreate(pixs);
        }
    }

    if (!ccb) {
        /* FIXME: create data for no pix? */
        DBG(LOG_NEW_CLASS, "%s: create for %s* = %p\n", _fun,
            LL_PIX, nullptr);
        ccb = ccbCreate(nullptr);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        LL_CCBORD, reinterpret_cast<void *>(ccb));
    return ll_push_CCBord(_fun, L, ccb);
}

/**
 * \brief Register the CCBord methods and functions in the LL_CCBORD meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_CCBord(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},       /* garbage collector */
        {"__new",               ll_new_CCBord}, /* CCBord(pix) */
        {"__tostring",          toString},
        {"Create",              Create},
        {"Destroy",             Destroy},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, ll_new_CCBord);
    lua_setglobal(L, LL_CCBORD);
    return ll_register_class(L, LL_CCBORD, methods, functions);
}
