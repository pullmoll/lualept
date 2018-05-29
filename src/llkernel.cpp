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
 *  Lua class Kernel
 *
 *====================================================================*/

/** Define a function's name (_fun) with prefix LL_KERNEL */
#define LL_FUNC(x) FUNC(LL_KERNEL "." x)

/**
 * \brief Destroy a Kernel* (%kel).
 * <pre>
 * Arg #1 is expected to be a l_int32 (freeflag).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Kernel **pkel = ll_check_udata<Kernel>(_fun, L, 1, LL_KERNEL);
    Kernel *kel = *pkel;
    DBG(LOG_DESTROY, "%s: '%s' pkel=%p kel=%p\n",
        _fun, LL_SEL, pkel, kel);
    kernelDestroy(&kel);
    *pkel = nullptr;
    return 0;
}

/**
 * \brief Create a Kernel* (%kel).
 * <pre>
 * Arg #1 is expected to be a l_int32 (height).
 * Arg #2 is expected to be a l_int32 (width).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Kernel* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 height = ll_check_l_int32(_fun, L, 1);
    l_int32 width = ll_check_l_int32(_fun, L, 2);
    Kernel *kel = kernelCreate(height, width);
    return ll_push_Kernel (_fun, L, kel);
}

/**
 * \brief Printable string for a Pix*.
 * \param L pointer to the lua_State
 * @return 1 string on the Lua stack
 */
static int
toString(lua_State* L)
{
    LL_FUNC("toString");
    char str[256];
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    l_int32 sy, sx, cy, cx, y, x;
    l_float32 sum;
    l_float32 val;
    int len;
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!kel) {
        luaL_addstring(&B, "nil");
    } else {
        if (kernelGetParameters(kel, &sy, &sx, &cy, &cx)) {
            snprintf(str, sizeof(str), "invalid");
        } else {
            kernelGetSum(kel, &sum);
            snprintf(str, sizeof(str),
                     LL_KERNEL ": %p\n"
                     "    sy = %d, sx = %d, cy = %d, cx = %d, sum = %g\n",
                     reinterpret_cast<void *>(kel),
                     sy, sx, cy, cx, static_cast<double>(sum));
        }
        luaL_addstring(&B, str);
        for (y = 0; y < sy; y++) {
            if (y > 0)
                luaL_addstring(&B, "\n");
            luaL_addstring(&B, "    ");
            for (x = 0; x < sx; x++) {
                if (x > 0) {
                    snprintf(str, sizeof(str), " %*s", 10 - len, "");
                    luaL_addstring(&B, str);
                }
                kernelGetElement(kel, y, x, &val);
                len = snprintf(str, sizeof(str), "%.6g", static_cast<double>(val));
                luaL_addstring(&B, str);
            }
        }
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Copy a Kernel* (%kels).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kels).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Kernel *kels = ll_check_Kernel(_fun, L, 1);
    Kernel *kel = kernelCopy(kels);
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Create a Kernel* (%kel) from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Kernel* on the Lua stack
 */
static int
CreateFromFile(lua_State *L)
{
    LL_FUNC("CreateFromFile");
    const char *filename = ll_check_string(_fun, L, 1);
    Kernel *kel = kernelCreateFromFile(filename);
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Create a Kernel* (%kel) from a Pix* (%pix).
 * <pre>
 * Arg #1 is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (cy).
 * Arg #3 is expected to be a l_int32 (cx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Kernel* on the Lua stack
 */
static int
CreateFromPix(lua_State *L)
{
    LL_FUNC("CreateFromPix");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 cy = ll_check_l_int32(_fun, L, 2);
    l_int32 cx = ll_check_l_int32(_fun, L, 3);
    Kernel *kel = kernelCreateFromPix(pix, cy, cx);
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Create a Kernel* (%kel) from a string (%kdata).
 * <pre>
 * Arg #1 is expected to be a l_int32 (h).
 * Arg #2 is expected to be a l_int32 (w).
 * Arg #3 is expected to be a l_int32 (cy).
 * Arg #4 is expected to be a l_int32 (cx).
 * Arg #5 is expected to be a string (kdata).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Kernel* on the Lua stack
 */
static int
CreateFromString(lua_State *L)
{
    LL_FUNC("CreateFromString");
    l_int32 h = ll_check_l_int32(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 cy = ll_check_l_int32(_fun, L, 3);
    l_int32 cx = ll_check_l_int32(_fun, L, 4);
    const char *kdata = ll_check_string(_fun, L, 5);
    Kernel *kel = kernelCreateFromString(h, w, cy, cx, kdata);
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Display a Kernel* (%kel) in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * Arg #2 is expected to be a l_int32 (size).
 * Arg #3 is expected to be a l_int32 (gthick).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix* on the Lua stack
 */
static int
DisplayInPix(lua_State *L)
{
    LL_FUNC("DisplayInPix");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    l_int32 size = ll_check_l_int32(_fun, L, 2);
    l_int32 gthick = ll_check_l_int32(_fun, L, 3);
    Pix *pix = kernelDisplayInPix(kel, size, gthick);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Get element in row (%row), column (%col) from a Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * Arg #2 is expected to be a l_int32 (row).
 * Arg #3 is expected to be a l_int32 (col).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 number on the Lua stack
 */
static int
GetElement(lua_State *L)
{
    LL_FUNC("GetElement");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    l_int32 row = ll_check_l_int32(_fun, L, 2);
    l_int32 col = ll_check_l_int32(_fun, L, 3);
    l_float32 val = 0;
    if (kernelGetElement(kel, row, col, &val))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, val);
    return 1;
}

/**
 * \brief Get the minimum and maximum values from a Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 numbers on the Lua stack
 */
static int
GetMinMax(lua_State *L)
{
    LL_FUNC("GetMinMax");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    l_float32 min = 0;
    l_float32 max = 0;
    if (kernelGetMinMax(kel, &min, &max))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, min);
    ll_push_l_float32(_fun, L, max);
    return 2;
}

/**
 * \brief Get the parameters for a Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack
 */
static int
GetParameters(lua_State *L)
{
    LL_FUNC("GetParameters");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    l_int32 sy = 0;
    l_int32 sx = 0;
    l_int32 cy = 0;
    l_int32 cx = 0;
    if (kernelGetParameters(kel, &sy, &sx, &cy, &cx))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, sy);
    ll_push_l_int32(_fun, L, sx);
    ll_push_l_int32(_fun, L, cy);
    ll_push_l_int32(_fun, L, cx);
    return 4;
}

/**
 * \brief Get the sum of a Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 number on the Lua stack
 */
static int
GetSum(lua_State *L)
{
    LL_FUNC("GetSum");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    l_float32 sum = 0;
    if (kernelGetSum(kel, &sum))
        return ll_push_nil(L);
    ll_push_l_float32(_fun, L, sum);
    return 1;
}

/**
 * \brief Invert a Kernel* (%kels).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kels).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Kernel* on the Lua stack
 */
static int
Invert(lua_State *L)
{
    LL_FUNC("Invert");
    Kernel *kels = ll_check_Kernel(_fun, L, 1);
    Kernel *kel = kernelInvert(kels);
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Normalize a Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kels).
 * Arg #2 is expected to be a l_float32 (normsum).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Normalize(lua_State *L)
{
    LL_FUNC("Normalize");
    Kernel *kels = ll_check_Kernel(_fun, L, 1);
    l_float32 normsum = ll_check_l_float32_default(_fun, L, 2, 1.0f);
    Kernel *kel = kernelNormalize(kels, normsum);
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Read a Kernel* (%kel) from an external file (%fname).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (fname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *fname = ll_check_string(_fun, L, 1);
    Kernel *kel = kernelRead(fname);
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Read a Kernel* (%kel) from a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Kernel *kel = kernelReadStream(stream->f);
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Set element in row (%row), column (%col) in a Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * Arg #2 is expected to be a l_int32 (row).
 * Arg #3 is expected to be a l_int32 (col).
 * Arg #4 is expected to be a l_float32 (val).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetElement(lua_State *L)
{
    LL_FUNC("SetElement");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    l_int32 row = ll_check_l_int32(_fun, L, 2);
    l_int32 col = ll_check_l_int32(_fun, L, 3);
    l_float32 val = ll_check_l_float32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == kernelSetElement(kel, row, col, val));
}

/**
 * \brief Set the origin (%cy, %cx) of a Kernel* (%kel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * Arg #2 is expected to be a l_int32 (cy).
 * Arg #3 is expected to be a l_int32 (cx).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetOrigin(lua_State *L)
{
    LL_FUNC("SetOrigin");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    l_int32 cy = ll_check_l_int32(_fun, L, 2);
    l_int32 cx = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == kernelSetOrigin(kel, cy, cx));
}

/**
 * \brief Write a Kernel* (%kel) to an external file (%fname).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * Arg #2 is expected to be a string (fname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    const char *fname = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == kernelWrite(fname, kel));
}

/**
 * \brief Write a Kernel* (%kel) to a luaL_Stream* (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Kernel* (kel).
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Kernel *kel = ll_check_Kernel(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == kernelWriteStream(stream->f, kel));
}

/**
 * \brief Check Lua stack at index (%arg) for udata of class LL_KERNEL.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Kernel* contained in the user data
 */
Kernel *
ll_check_Kernel(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Kernel>(_fun, L, arg, LL_KERNEL);
}

/**
 * \brief Optionally expect a Kernel* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Kernel* contained in the user data
 */
Kernel *
ll_check_Kernel_opt(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Kernel(_fun, L, arg);
}

/**
 * \brief Push Kernel* to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param cd pointer to the L_Kernel
 * \return 1 Kernel* on the Lua stack
 */
int
ll_push_Kernel(const char *_fun, lua_State *L, Kernel *cd)
{
    if (!cd)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, LL_KERNEL, cd);
}

/**
 * \brief Create and push a new Kernel*.
 * \param L pointer to the lua_State
 * \return 1 Kernel* on the Lua stack
 */
int
ll_new_Kernel(lua_State *L)
{
    return Create(L);
}

/**
 * \brief Register the Kernel methods and functions in the LL_KERNEL meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_register_Kernel(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},   /* garbage collector */
        {"__new",               Create},
        {"__tostring",          toString},
        {"Copy",                Copy},
        {"Create",              Create},
        {"CreateFromFile",      CreateFromFile},
        {"CreateFromPix",       CreateFromPix},
        {"CreateFromString",    CreateFromString},
        {"Destroy",             Destroy},
        {"DisplayInPix",        DisplayInPix},
        {"GetElement",          GetElement},
        {"GetMinMax",           GetMinMax},
        {"GetParameters",       GetParameters},
        {"GetSum",              GetSum},
        {"Invert",              Invert},
        {"Normalize",           Normalize},
        {"Read",                Read},
        {"ReadStream",          ReadStream},
        {"SetElement",          SetElement},
        {"SetOrigin",           SetOrigin},
        {"Write",               Write},
        {"WriteStream",         WriteStream},
        LUA_SENTINEL
    };

    static const luaL_Reg functions[] = {
        LUA_SENTINEL
    };

    lua_pushcfunction(L, Create);
    lua_setglobal(L, LL_KERNEL);
    return ll_register_class(L, LL_KERNEL, methods, functions);
}