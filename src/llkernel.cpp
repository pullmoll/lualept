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
 * \file llkernel.cpp
 * \class Kernel
 *
 * A kernel.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_KERNEL

/** Define a function's name (_fun) with prefix Kernel */
#define LL_FUNC(x) FUNC(TNAME "." x)

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
    Kernel **pkel = ll_check_udata<Kernel>(_fun, L, 1, TNAME);
    Kernel *kel = *pkel;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p\n", _fun,
        TNAME,
        "pkel", reinterpret_cast<void *>(pkel),
        "kel", reinterpret_cast<void *>(kel));
    kernelDestroy(&kel);
    *pkel = nullptr;
    return 0;
}

/**
 * \brief Printable string for a Pix*.
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
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
                     TNAME ": %p\n"
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
 * \brief Create a Kernel* (%kel).
 * <pre>
 * Arg #1 is expected to be a l_int32 (height).
 * Arg #2 is expected to be a l_int32 (width).
 *
 * Leptonica's Notes:
 *      (1) kernelCreate() initializes all values to 0.
 *      (2) After this call, (cy,cx) and nonzero data values must be
 *          assigned.
 *      (2) The number of kernel elements must be less than 2^29.
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
 * \brief Create a Kernel* (%kel) from an external file (%filename).
 * <pre>
 * Arg #1 is expected to be a string (filename).
 *
 * Leptonica's Notes:
 *      (1) The file contains, in the following order:
 *           ~ Any number of comment lines starting with '#' are ignored
 *           ~ The height and width of the kernel
 *           ~ The y and x values of the kernel origin
 *           ~ The kernel data, formatted as lines of numbers (integers
 *             or floats) for the kernel values in row-major order,
 *             and with no other punctuation.
 *             (Note: this differs from kernelCreateFromString(),
 *             where each line must begin and end with a double-quote
 *             to tell the compiler it's part of a string.)
 *           ~ The kernel specification ends when a blank line,
 *             a comment line, or the end of file is reached.
 *      (2) All lines must be left-justified.
 *      (3) See kernelCreateFromString() for a description of the string
 *          format for the kernel data.  As an example, here are the lines
 *          of a valid kernel description file  In the file, all lines
 *          are left-justified:
 * \code
 *                    # small 3x3 kernel
 *                    3 3
 *                    1 1
 *                    25.5   51    24.3
 *                    70.2  146.3  73.4
 *                    20     50.9  18.4
 * \endcode
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
 *
 * Leptonica's Notes:
 *      (1) The origin must be positive and within the dimensions of the pix.
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
 *
 * Leptonica's Notes:
 *      (1) The data is an array of chars, in row-major order, giving
 *          space separated integers in the range [-255 ... 255].
 *      (2) The only other formatting limitation is that you must
 *          leave space between the last number in each row and
 *          the double-quote.  If possible, it's also nice to have each
 *          line in the string represent a line in the kernel; e.g.,
 *              static const char *kdata =
 *                  " 20   50   20 "
 *                  " 70  140   70 "
 *                  " 20   50   20 ";
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
 *
 * Leptonica's Notes:
 *      (1) This gives a visual representation of a kernel.
 *      (2) There are two modes of display:
 *          (a) Grid lines of minimum width 2, surrounding regions
 *              representing kernel elements of minimum size 17,
 *              with a "plus" mark at the kernel origin, or
 *          (b) A pix without grid lines and using 1 pixel per kernel element.
 *      (3) For both cases, the kernel absolute value is displayed,
 *          normalized such that the maximum absolute value is 255.
 *      (4) Large 2D separable kernels should be used for convolution
 *          with two 1D kernels.  However, for the bilateral filter,
 *          the computation time is independent of the size of the
 *          2D content kernel.
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
 *
 * Leptonica's Notes:
 *      (1) For convolution, the kernel is spatially inverted before
 *          a "correlation" operation is done between the kernel and the image.
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
 *
 * Leptonica's Notes:
 *      (1) If the sum of kernel elements is close to 0, do not
 *          try to calculate the normalized kernel.  Instead,
 *          return a copy of the input kernel, with a warning.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 on the Lua stack
 */
static int
Normalize(lua_State *L)
{
    LL_FUNC("Normalize");
    Kernel *kels = ll_check_Kernel(_fun, L, 1);
    l_float32 normsum = ll_opt_l_float32(_fun, L, 2, 1.0f);
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
 * \brief Check Lua stack at index (%arg) for udata of class Kernel*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Kernel* contained in the user data
 */
Kernel *
ll_check_Kernel(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Kernel>(_fun, L, arg, TNAME);
}

/**
 * \brief Optionally expect a Kernel* at index (%arg) on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Kernel* contained in the user data
 */
Kernel *
ll_opt_Kernel(const char *_fun, lua_State *L, int arg)
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
    return ll_push_udata(_fun, L, TNAME, cd);
}

/**
 * \brief Create and push a new Kernel*.
 * \param L pointer to the lua_State
 * \return 1 Kernel* on the Lua stack
 */
int
ll_new_Kernel(lua_State *L)
{
    FUNC("ll_new_Kernel");
    Kernel* kel = nullptr;
    Pix *pixs = nullptr;
    luaL_Stream *stream = nullptr;
    l_int32 width = 2;
    l_int32 height = 2;
    l_int32 cy = 0;
    l_int32 cx = 0;

    if (lua_isuserdata(L, 1)) {
        pixs = ll_opt_Pix(_fun, L, 1);
        if (pixs) {
            cy = ll_opt_l_int32(_fun, L, 2, cy);
            cx = ll_opt_l_int32(_fun, L, 2, cx);
            DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
                LL_PIX, reinterpret_cast<void *>(pixs));
            kel = kernelCreateFromPix(pixs, cy, cx);
        } else {
            stream = ll_check_stream(_fun, L, 1);
            DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
                LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
            kel = kernelReadStream(stream->f);
        }
    }

    if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        height = ll_opt_l_int32(_fun, L, 1, width);
        width = ll_opt_l_int32(_fun, L, 2, height);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "height", height, "width", width);
        kel = kernelCreate(height, width);
    }

    if (!kel && lua_isstring(L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        kel = kernelRead(filename);
    }

    if (!kel && lua_isstring(L, 1)) {
        height = ll_opt_l_int32(_fun, L, 1, height);
        width = ll_opt_l_int32(_fun, L, 2, width);
        cy = ll_opt_l_int32(_fun, L, 3, cy);
        cx = ll_opt_l_int32(_fun, L, 4, cx);
        const char* kdata = ll_check_string(_fun, L, 5);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = %d, %s = %d, %s = %s\n", _fun,
            "height", height,
            "width", width,
            "cy", cy,
            "cx", cx,
            "kdata", kdata);
        kel = kernelCreateFromString(height, width, cy, cx, kdata);
    }

    if (!kel) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d\n", _fun,
            "height", height, "width", width);
        kel = kernelCreate(height, width);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(kel));
    return ll_push_Kernel(_fun, L, kel);
}

/**
 * \brief Register the Kernel methods and functions in the Kernel meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
luaopen_Kernel(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                Destroy},
        {"__new",               ll_new_Kernel},
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
    LO_FUNC(TNAME);
    ll_global_cfunct(_fun, L, TNAME, ll_new_Kernel);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
