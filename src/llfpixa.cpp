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
 *  Lua class FPixa
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_FPIXA */
#define LL_FUNC(x) FUNC(LL_FPIXA "." x)

/**
 * \brief Destroy a FPixa*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    FPixa **pfpixa = ll_check_udata<FPixa>(_fun, L, 1, LL_FPIXA);
    FPixa *fpixa = *pfpixa;
    DBG(LOG_DESTROY, "%s: '%s' pfpixa=%p fpixa=%p count=%d\n",
        _fun, LL_FPIXA, pfpixa, fpixa, fpixaGetCount(fpixa));
    fpixaDestroy(&fpixa);
    *pfpixa = nullptr;
    return 0;
}

/**
 * \brief Create a new FPixa*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (n).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPixa* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
    FPixa *fpixa = fpixaCreate(n);
    return ll_push_FPixa(_fun, L, fpixa);
}

/**
 * \brief Get the number of FPix* in FPixa* (%fpixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 integer on the Lua stack
 */
static int
GetCount(lua_State *L)
{
    LL_FUNC("GetCount");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    return ll_push_l_int32(_fun, L, fpixaGetCount(fpixa));
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be a FPix* (fpix).
 * Arg #3 is expected to be a l_int32 (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
AddFPix(lua_State *L)
{
    LL_FUNC("AddFPix");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    FPix *fpix = ll_check_FPix(_fun, L, 2);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 3, L_COPY);
    return ll_push_boolean(_fun, L, 0 == fpixaAddFPix(fpixa, fpix, copyflag));
}

/**
 * \brief Adjust the reference count of FPixa* (%fpixa) by delta (%delta).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be a l_int32 (delta).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
ChangeRefcount(lua_State *L)
{
    LL_FUNC("ChangeRefcount");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    l_int32 delta = ll_check_l_int32(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == fpixaChangeRefcount(fpixa, delta));
}

/**
 * \brief Convert FPix* in FPixa* (%fpixas) from L*a*b* to RGB.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPixa* on the Lua stack
 */
static int
ConvertLABToRGB(lua_State *L)
{
    LL_FUNC("ConvertLABToRGB");
    FPixa *fpixas = ll_check_FPixa(_fun, L, 1);
    Pix *fpixa = fpixaConvertLABToRGB(fpixas);
    return ll_push_Pix(_fun, L, fpixa);
}

/**
 * \brief Convert FPix* in FPixa* (%fpixas) from L*a*b* to XYZ.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPixa* on the Lua stack
 */
static int
ConvertLABToXYZ(lua_State *L)
{
    LL_FUNC("ConvertLABToXYZ");
    FPixa *fpixas = ll_check_FPixa(_fun, L, 1);
    FPixa *fpixa = fpixaConvertLABToXYZ(fpixas);
    return ll_push_FPixa(_fun, L, fpixa);
}

/**
 * \brief Convert FPix* in FPixa* (%fpixas) from XYZ to L*a*b*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixas).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPixa* on the Lua stack
 */
static int
ConvertXYZToLAB(lua_State *L)
{
    LL_FUNC("ConvertXYZToLAB");
    FPixa *fpixas = ll_check_FPixa(_fun, L, 1);
    FPixa *fpixa = fpixaConvertXYZToLAB(fpixas);
    return ll_push_FPixa(_fun, L, fpixa);
}

/**
 * \brief Convert FPix* in FPixa* (%fpixas) from XYZ to a single RGB Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
ConvertXYZToRGB(lua_State *L)
{
    LL_FUNC("ConvertXYZToRGB");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    Pix *pix = fpixaConvertXYZToRGB(fpixa);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Copy FPix* (%fpixas) to FPixa* (%fpixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be a string describing the copy mode (copyflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPixa* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    FPixa *fpixas = ll_check_FPixa(_fun, L, 1);
    l_int32 copyflag = ll_check_access_storage(_fun, L, 2, L_COPY);
    FPixa *fpixa = fpixaCopy(fpixas, copyflag);
    return ll_push_FPixa(_fun, L, fpixa);
}

/**
 * \brief Display the quad tree for FPix* in FPixa* (%fpixa) in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be a l_int32 (factor).
 * Arg #3 is expected to be a l_int32 (fontsize).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
DisplayQuadtree(lua_State *L)
{
    LL_FUNC("DisplayQuadtree");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    l_int32 factor = ll_check_l_int32(_fun, L, 2);
    l_int32 fontsize = ll_check_l_int32(_fun, L, 3);
    Pix *pix = fpixaDisplayQuadtree(fpixa, factor, fontsize);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get the data for FPix* at index (%idx) from FPixa* (%fpixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
GetData(lua_State *L)
{
    LL_FUNC("GetData");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, fpixaGetCount(fpixa));
    FPix *fpix = fpixaGetFPix(fpixa, idx, L_CLONE);
    l_int32 wpl = fpixGetWpl(fpix);
    l_int32 w = 0;
    l_int32 h = 0;
    l_float32 *farray = nullptr;
    if (fpixaGetFPixDimensions(fpixa, idx, &w, &h))
        return ll_push_nil(L);
    farray = fpixaGetData(fpixa, idx);
    fpixDestroy(&fpix);
    return ll_push_Farray_2d(_fun, L, farray, wpl, h);
}

/**
 * \brief Get the FPix* at index (%idx) from FPixa* (%fpixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be a l_int32 (idx).
 * Arg #3 is expected to be a l_int32 (accesstype).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 FPix* on the Lua stack
 */
static int
GetFPix(lua_State *L)
{
    LL_FUNC("GetFPix");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, fpixaGetCount(fpixa));
    l_int32 accesstype = ll_check_access_storage(_fun, L, 3, L_COPY);
    FPix *fpix = fpixaGetFPix(fpixa, idx, accesstype);
    return ll_push_FPix(_fun, L, fpix);
}

/**
 * \brief Get the dimensions of FPix* at index (%idx) from FPixa* (%fpixa).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be a l_int32 (idx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 integers (%w, %h) on the Lua stack
 */
static int
GetFPixDimensions(lua_State *L)
{
    LL_FUNC("GetFPixDimensions");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, fpixaGetCount(fpixa));
    l_int32 w = 0;
    l_int32 h = 0;
    if (fpixaGetFPixDimensions(fpixa, idx, &w, &h))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, w);
    ll_push_l_int32(_fun, L, h);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be an index (idx).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_float32 (%val) on the Lua stack
 */
static int
GetPixel(lua_State *L)
{
    LL_FUNC("GetPixel");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    l_int32 idx = ll_check_index(_fun, L, 2, fpixaGetCount(fpixa));
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_float32 val = 0;
    if (fpixaGetPixel(fpixa, idx, x, y, &val))
        return ll_push_nil(L);
    return ll_push_l_float32(_fun, L, val);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a FPixa* (fpixa).
 * Arg #2 is expected to be an index (idx).
 * Arg #3 is expected to be a l_int32 (x).
 * Arg #4 is expected to be a l_int32 (y).
 * Arg #5 is expected to be a l_float32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetPixel(lua_State *L)
{
    LL_FUNC("SetPixel");
    FPixa *fpixa = ll_check_FPixa(_fun, L, 1);
    l_int32 index = ll_check_index(_fun, L, 2, fpixaGetCount(fpixa));
    l_int32 x = ll_check_l_int32(_fun, L, 3);
    l_int32 y = ll_check_l_int32(_fun, L, 4);
    l_float32 val = ll_check_l_float32(_fun, L, 5);
    return ll_push_boolean(_fun, L, 0 == fpixaSetPixel(fpixa, index, x, y, val));
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class LL_FPIXA.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the FPixa* contained in the user data
 */
FPixa *
ll_check_FPixa(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<FPixa>(_fun, L, arg, LL_FPIXA);
}
/**
 * \brief Optionally expect a FPixa* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the FPixa* contained in the user data
 */
FPixa *
ll_opt_FPixa(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_FPixa(_fun, L, arg);
}
/**
 * \brief Push FPixa* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_FPixa
 * \return 1 FPixa* on the Lua stack
 */
int
ll_push_FPixa(const char *_fun, lua_State *L, FPixa *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_FPIXA, cd);
}
/**
 * \brief Create and push a new FPixa*.
 *
 * Arg #1 is expected to be a string (dir).
 * Arg #2 is expected to be a l_int32 (fontsize).
 *
 * \param L pointer to the lua_State
 * \return 1 FPixa* on the Lua stack
 */
int
ll_new_FPixa(lua_State *L)
{
    FUNC("ll_new_FPixa");
    if (lua_isuserdata(L, 1)) {
        FPixa *fpixas = ll_check_FPixa(_fun, L, 1);
        FPixa *fpixa = fpixaCopy(fpixas, L_COPY);
        return ll_push_FPixa(_fun, L, fpixa);
    }
    if (lua_isinteger(L, 1)) {
        l_int32 n = ll_opt_l_int32(_fun, L, 1, 1);
        FPixa *fpixa = fpixaCreate(n);
        return ll_push_FPixa(_fun, L, fpixa);
    }
    return ll_push_nil(L);
}

/**
 * \brief Register the FPixa methods and functions in the LL_FPIXA meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_FPixa(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},           /* garbage collector */
        {"__new",               ll_new_FPixa},      /* FPixa() */
        {"__len",               GetCount},
        {"AddFPix",             AddFPix},
        {"ChangeRefcount",      ChangeRefcount},
        {"ConvertLABToRGB",     ConvertLABToRGB},
        {"ConvertLABToXYZ",     ConvertLABToXYZ},
        {"ConvertXYZToLAB",     ConvertXYZToLAB},
        {"ConvertXYZToRGB",     ConvertXYZToRGB},
        {"Copy",                Copy},
        {"Create",              Create},
        {"Destroy",             Destroy},
        {"DisplayQuadtree",     DisplayQuadtree},
        {"GetCount",            GetCount},
        {"GetData",             GetData},
        {"GetFPix",             GetFPix},
        {"GetFPixDimensions",   GetFPixDimensions},
        {"GetPixel",            GetPixel},
        {"SetPixel",            SetPixel},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, ll_new_Pixa);
    lua_setglobal(L, LL_FPIXA);
    return ll_register_class(L, LL_FPIXA, methods, functions);
}
