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
 * \file llsel.cpp
 * \class Sel
 *
 * A class handling SEL.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_SEL

/** Define a function's name (_fun) with prefix Sel */
#define LL_FUNC(x) FUNC(TNAME "." x)

/**
 * \brief Destroy a Sel*.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 0 for nothing on the Lua stack
 */
static int
Destroy(lua_State *L)
{
    LL_FUNC("Destroy");
    Sel **psel = ll_check_udata<Sel>(_fun, L, 1, TNAME);
    Sel *sel = *psel;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p\n", _fun,
        TNAME,
        "psel", reinterpret_cast<void *>(psel),
        "sel", reinterpret_cast<void *>(sel));
    selDestroy(&sel);
    *psel = nullptr;
    return 0;
}

/**
 * \brief Printable string for a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
toString(lua_State *L)
{
    LL_FUNC("toString");
    char str[256];
    Sel *sel = ll_check_Sel(_fun, L, 1);
    luaL_Buffer B;

    luaL_buffinit(L, &B);
    if (!sel) {
        luaL_addstring(&B, "nil");
    } else {
        snprintf(str, sizeof(str),
                 TNAME ": %p\n",
                 reinterpret_cast<void *>(sel));
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    return 1;
}

/**
 * \brief Copy a Sel* (%sels).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel* on the Lua stack
 */
static int
Copy(lua_State *L)
{
    LL_FUNC("Copy");
    Sel *sels = ll_check_Sel(_fun, L, 1);
    Sel *sel = selCopy(sels);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Create a new Sel*.
 * <pre>
 * Arg #1 is expected to be a l_int32 (height).
 * Arg #2 is expected to be a l_int32 (width).
 * Arg #3 is expected to be a string (name).
 *
 * Leptonica's Notes:
 *      (1) selCreate() initializes all values to 0.
 *      (2) After this call, (cy,cx) and nonzero data values must be
 *          assigned.  If a text name is not assigned here, it will
 *          be needed later when the sel is put into a sela.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel* on the Lua stack
 */
static int
Create(lua_State *L)
{
    LL_FUNC("Create");
    l_int32 height = ll_opt_l_int32(_fun, L, 1, 1);
    l_int32 width = ll_opt_l_int32(_fun, L, 2, 1);
    const char *name = ll_check_string(_fun, L, 3);
    Sel *pa = selCreate(height, width, name);
    return ll_push_Sel(_fun, L, pa);
}

/**
 * \brief Create a brick Sel* (%sel).
 * <pre>
 *
 * Leptonica's Notes:
 *      (1) This is a rectangular sel of all hits, misses or don't cares.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
CreateBrick(lua_State *L)
{
    LL_FUNC("CreateBrick");
    l_int32 h = ll_check_l_int32(_fun, L, 1);
    l_int32 w = ll_check_l_int32(_fun, L, 2);
    l_int32 cy = ll_check_l_int32(_fun, L, 3);
    l_int32 cx = ll_check_l_int32(_fun, L, 4);
    l_int32 type = ll_check_sel(_fun, L, 5, SEL_DONT_CARE);
    Sel *sel = selCreateBrick(h, w, cy, cx, type);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Create a comb Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a l_int32 (factor1).
 * Arg #2 is expected to be a l_int32 (factor2).
 * Arg #3 is expected to be a l_int32 (direction).
 *
 * Leptonica's Notes:
 *      (1) This generates a comb Sel of hits with the origin as
 *          near the center as possible.
 *      (2) In use, this is complemented by a brick sel of size %factor1,
 *          Both brick and comb sels are made by selectComposableSels().
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
CreateComb(lua_State *L)
{
    LL_FUNC("CreateComb");
    l_int32 factor1 = ll_check_l_int32(_fun, L, 1);
    l_int32 factor2 = ll_check_l_int32(_fun, L, 2);
    l_int32 direction = ll_check_l_int32(_fun, L, 3);
    Sel *sel = selCreateComb(factor1, factor2, direction);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Create a Sel* (%sel) from a color Pix* (%pixs).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pixs).
 * Arg #2 is expected to be a char* (selname).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
CreateFromColorPix(lua_State *L)
{
    LL_FUNC("CreateFromColorPix");
    Pix *pixs = ll_check_Pix(_fun, L, 1);
    const char *name = ll_check_string(_fun, L, 2);
    /* XXX: deconstify */
    char *selname = reinterpret_cast<char *>(reinterpret_cast<l_intptr_t>(name));
    Sel *sel = selCreateFromColorPix(pixs, selname);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Create a Sel* (%sel) from a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pix* (pix).
 * Arg #2 is expected to be a l_int32 (cy).
 * Arg #3 is expected to be a l_int32 (cx).
 * Arg #4 is expected to be a string (name).
 *
 * Leptonica's Notes:
 *      (1) The origin must be positive.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
CreateFromPix(lua_State *L)
{
    LL_FUNC("CreateFromPix");
    Pix *pix = ll_check_Pix(_fun, L, 1);
    l_int32 cy = ll_check_l_int32(_fun, L, 2);
    l_int32 cx = ll_check_l_int32(_fun, L, 3);
    const char *name = ll_check_string(_fun, L, 4);
    Sel *sel = selCreateFromPix(pix, cy, cx, name);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Create a Sel* (%sel) from a Pta* (%pta).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Pta* (pta).
 * Arg #2 is expected to be a l_int32 (cy).
 * Arg #3 is expected to be a l_int32 (cx).
 * Arg #4 is expected to be a string (name).
 *
 * Leptonica's Notes:
 *      (1) The origin and all points in the pta must be positive.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
CreateFromPta(lua_State *L)
{
    LL_FUNC("CreateFromPta");
    Pta *pta = ll_check_Pta(_fun, L, 1);
    l_int32 cy = ll_check_l_int32(_fun, L, 2);
    l_int32 cx = ll_check_l_int32(_fun, L, 3);
    const char *name = ll_check_string(_fun, L, 4);
    Sel *sel = selCreateFromPta(pta, cy, cx, name);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Create a Sel* (%sel) from a string (%text).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a string (text).
 * Arg #2 is expected to be a l_int32 (h).
 * Arg #3 is expected to be a l_int32 (w).
 * Arg #4 is expected to be a string (name).
 *
 * Leptonica's Notes:
 *      (1) The text is an array of chars (in row-major order) where
 *          each char can be one of the following:
 *             'x': hit
 *             'o': miss
 *             ' ': don't-care
 *      (2) When the origin falls on a hit or miss, use an upper case
 *          char (e.g., 'X' or 'O') to indicate it.  When the origin
 *          falls on a don't-care, indicate this with a 'C'.
 *          The string must have exactly one origin specified.
 *      (3) The advantage of this method is that the text can be input
 *          in a format that shows the 2D layout of the Sel; e.g.,
 * \code
 *              static const char *seltext = "x    "
 *                                           "x Oo "
 *                                           "x    "
 *                                           "xxxxx";
 * \endcode
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel * on the Lua stack
 */
static int
CreateFromString(lua_State *L)
{
    LL_FUNC("CreateFromString");
    const char *text = ll_check_string(_fun, L, 1);
    l_int32 h = ll_check_l_int32(_fun, L, 2);
    l_int32 w = ll_check_l_int32(_fun, L, 3);
    const char *name = ll_check_string(_fun, L, 4);
    Sel *sel = selCreateFromString(text, h, w, name);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Display a Sel* (%sel) in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * Arg #2 is expected to be a l_int32 (size).
 * Arg #3 is expected to be a l_int32 (gthick).
 *
 * Leptonica's Notes:
 *      (1) This gives a visual representation of a general (hit-miss) sel.
 *      (2) The empty sel is represented by a grid of intersecting lines.
 *      (3) Three different patterns are generated for the sel elements:
 *          ~ hit (solid black circle)
 *          ~ miss (black ring; inner radius is radius2)
 *          ~ origin (cross, XORed with whatever is there)
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Pix * on the Lua stack
 */
static int
DisplayInPix(lua_State *L)
{
    LL_FUNC("DisplayInPix");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    l_int32 size = ll_check_l_int32(_fun, L, 2);
    l_int32 gthick = ll_check_l_int32(_fun, L, 3);
    Pix *pix = selDisplayInPix(sel, size, gthick);
    return ll_push_Pix(_fun, L, pix);
}

/**
 * \brief Find the max translations for a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 *
 * Leptonica's Notes:
          These are the maximum shifts for the erosion operation.
 *        For example, when j < cx, the shift of the image
 *        is +x to the cx.  This is a positive xp shift.
 * </pre>
 * \param L pointer to the lua_State
 * \return 4 integers on the Lua stack
 */
static int
FindMaxTranslations(lua_State *L)
{
    LL_FUNC("FindMaxTranslations");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    l_int32 xp = 0;
    l_int32 yp = 0;
    l_int32 xn = 0;
    l_int32 yn = 0;
    if (selFindMaxTranslations(sel, &xp, &yp, &xn, &yn))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, xp);
    ll_push_l_int32(_fun, L, yp);
    ll_push_l_int32(_fun, L, xn);
    ll_push_l_int32(_fun, L, yn);
    return 4;
}

/**
 * \brief Get an element from a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * Arg #2 is expected to be a l_int32 (row).
 * Arg #3 is expected to be a l_int32 (col).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetElement(lua_State *L)
{
    LL_FUNC("GetElement");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    l_int32 row = ll_check_l_int32(_fun, L, 2);
    l_int32 col = ll_check_l_int32(_fun, L, 3);
    l_int32 type = SEL_DONT_CARE;
    if (selGetElement(sel, row, col, &type))
        return ll_push_nil(L);
    lua_pushstring(L, ll_string_sel(type));
    return 1;
}

/**
 * \brief Get the name of a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetName(lua_State *L)
{
    LL_FUNC("GetName");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    char *name = selGetName(sel);
    lua_pushstring(L, name);
    ll_free(name);
    return 1;
}

/**
 * \brief Get the parameters for a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 l_int32 on the Lua stack
 */
static int
GetParameters(lua_State *L)
{
    LL_FUNC("GetParameters");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    l_int32 sy = 0;
    l_int32 sx = 0;
    l_int32 cy = 0;
    l_int32 cx = 0;
    if (selGetParameters(sel, &sy, &sx, &cy, &cx))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, sy);
    ll_push_l_int32(_fun, L, sx);
    ll_push_l_int32(_fun, L, cy);
    ll_push_l_int32(_fun, L, cx);
    return 4;
}

/**
 * \brief Get the type at the origin of a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 string on the Lua stack
 */
static int
GetTypeAtOrigin(lua_State *L)
{
    LL_FUNC("GetTypeAtOrigin");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    l_int32 type = SEL_DONT_CARE;
    if (selGetTypeAtOrigin(sel, &type))
        return ll_push_nil(L);
    lua_pushstring(L, ll_string_sel(type));
    return 1;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 *
 * Leptonica's Notes:
 *      (1) This is an inverse function of selCreateFromString.
 *          It prints a textual representation of the SEL to a malloc'd
 *          string.  The format is the same as selCreateFromString
 *          except that newlines are inserted into the output
 *          between rows.
 *      (2) This is useful for debugging.  However, if you want to
 *          save some Sels in a file, put them in a Sela and write
 *          them out with selaWrite().  They can then be read in
 *          with selaRead().
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 char * on the Lua stack
 */
static int
PrintToString(lua_State *L)
{
    LL_FUNC("PrintToString");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    char *str = selPrintToString(sel);
    lua_pushstring(L, str);
    ll_free(str);
    return 1;
}

/**
 * \brief Read a Sel* (%sel) from an external file.
 * <pre>
 * Arg #1 is expected to be a string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
static int
Read(lua_State *L)
{
    LL_FUNC("Read");
    const char *filename = ll_check_string(_fun, L, 1);
    Sel *sel = selRead(filename);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Read a Sel* from a Lua string (%data).
 * <pre>
 * Arg #1 is expected to be a string (data).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sela* on the Lua stack
 */
static int
ReadFromColorImage(lua_State *L)
{
    LL_FUNC("ReadFromColorImage");
    const char *pathname = ll_check_string(_fun, L, 1);
    Sel *sel = selReadFromColorImage(pathname);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Read a Sel* from a Lua io stream (%stream).
 * <pre>
 * Arg #1 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel* on the Lua stack
 */
static int
ReadStream(lua_State *L)
{
    LL_FUNC("ReadStream");
    luaL_Stream *stream = ll_check_stream(_fun, L, 1);
    Sel *sel = selReadStream(stream->f);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Rotate a Sel* (%sels) orthogonally by %quads 90 degress cw.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sels).
 * Arg #2 is expected to be a l_int32 (quads).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 Sel* on the Lua stack
 */
static int
RotateOrth(lua_State *L)
{
    LL_FUNC("RotateOrth");
    Sel *sels = ll_check_Sel(_fun, L, 1);
    l_int32 quads = ll_check_rotation(_fun, L, 2, 0);
    Sel *sel = selRotateOrth(sels, quads);
    return ll_push_Sel(_fun, L, sel);
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a l_int32 (size).
 * Arg #2 is expected to be a l_int32 (direction).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 Sel* (%sel1, %sel2) on the Lua stack
 */
static int
SelectComposableSels(lua_State *L)
{
    LL_FUNC("SelectComposableSels");
    l_int32 size = ll_check_l_int32(_fun, L, 1);
    l_int32 direction = ll_check_l_int32(_fun, L, 2);
    Sel *sel1 = nullptr;
    Sel *sel2 = nullptr;
    if (selectComposableSels(size, direction, &sel1, &sel2))
        return ll_push_nil(L);
    ll_push_Sel(_fun, L, sel1);
    ll_push_Sel(_fun, L, sel2);
    return 2;
}

/**
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 is expected to be a l_int32 (size).
 * </pre>
 * \param L pointer to the lua_State
 * \return 2 integers on the Lua stack
 */
static int
SelectComposableSizes(lua_State *L)
{
    LL_FUNC("SelectComposableSizes");
    l_int32 size = ll_check_l_int32(_fun, L, 1);
    l_int32 factor1 = 0;
    l_int32 factor2 = 0;
    if (selectComposableSizes(size, &factor1, &factor2))
        return ll_push_nil(L);
    ll_push_l_int32(_fun, L, factor1);
    ll_push_l_int32(_fun, L, factor2);
    return 2;
}

/**
 * \brief Set an element of a Sel* (%sel) in row (%row), column (%col) to type (%type).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * Arg #2 is expected to be a l_int32 (row).
 * Arg #3 is expected to be a l_int32 (col).
 * Arg #4 is expected to be a l_int32 (type).
 *
 * Leptonica's Notes:
 *      (1) Because we use row and column to index into an array,
 *          they are always non-negative.  The location of the origin
 *          (and the type of operation) determine the actual
 *          direction of the rasterop.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetElement(lua_State *L)
{
    LL_FUNC("SetElement");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    l_int32 row = ll_check_l_int32(_fun, L, 2);
    l_int32 col = ll_check_l_int32(_fun, L, 3);
    l_int32 type = ll_check_l_int32(_fun, L, 4);
    return ll_push_boolean(_fun, L, 0 == selSetElement(sel, row, col, type));
}

/**
 * \brief Set the name of a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
 * Arg #2 is expected to be a string (name).
 *
 * Leptonica's Notes:
 *      (1) Always frees the existing sel name, if defined.
 *      (2) If name is not defined, just clears any existing sel name.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
SetName(lua_State *L)
{
    LL_FUNC("SetName");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    const char *name = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == selSetName(sel, name));
}

/**
 * \brief Set the origin of a Sel* (%sel).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sel* (sel).
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
    Sel *sel = ll_check_Sel(_fun, L, 1);
    l_int32 cy = ll_check_l_int32(_fun, L, 2);
    l_int32 cx = ll_check_l_int32(_fun, L, 3);
    return ll_push_boolean(_fun, L, 0 == selSetOrigin(sel, cy, cx));
}

/**
 * \brief Write the Sel* (%sel) to an external file (%filename).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* user data.
 * Arg #2 is expected to be string containing the filename.
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
Write(lua_State *L)
{
    LL_FUNC("Write");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    const char *filename = ll_check_string(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == selWrite(filename, sel));
}

/**
 * \brief Write the Sel* (%sel) to a Lua io stream (%stream).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a Sela* user data.
 * Arg #2 is expected to be a luaL_Stream* (stream).
 * </pre>
 * \param L pointer to the lua_State
 * \return 1 boolean on the Lua stack
 */
static int
WriteStream(lua_State *L)
{
    LL_FUNC("WriteStream");
    Sel *sel = ll_check_Sel(_fun, L, 1);
    luaL_Stream *stream = ll_check_stream(_fun, L, 2);
    return ll_push_boolean(_fun, L, 0 == selWriteStream(stream->f, sel));
}

/**
 * \brief Check Lua stack at index %arg for udata of class Sel*.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Sel* contained in the user data
 */
Sel *
ll_check_Sel(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<Sel>(_fun, L, arg, TNAME);
}
/**
 * \brief Optionally expect a Sel* at index %arg on the Lua stack.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the Sel* contained in the user data
 */
Sel *
ll_opt_Sel(const char *_fun, lua_State *L, int arg)
{
    if (!lua_isuserdata(L, arg))
        return nullptr;
    return ll_check_Sel(_fun, L, arg);
}
/**
 * \brief Push PTA user data to the Lua stack and set its meta table.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param sel pointer to the PTA
 * \return 1 Sel* on the Lua stack
 */
int
ll_push_Sel(const char *_fun, lua_State *L, Sel *sel)
{
    if (!sel)
        return ll_push_nil(L);
    return ll_push_udata(_fun, L, TNAME, sel);
}
/**
 * \brief Create and push a new Sel*.
 * \param L pointer to the lua_State
 * \return 1 Sel* on the Lua stack
 */
int
ll_new_Sel(lua_State *L)
{
    FUNC("ll_new_Sel");
    Sel *sel = nullptr;
    Sel *sels = nullptr;
    luaL_Stream *stream = nullptr;
    const char* name = "sel";
    l_int32 height = 3;
    l_int32 width = 3;

    if (lua_isuserdata(L, 1)) {
        sels = ll_opt_Sel(_fun, L, 1);
        if (sels) {
            DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
                TNAME, reinterpret_cast<void *>(sels));
            sel = selCopy(sels);
        } else {
            stream = ll_opt_stream(_fun, L, 1);
            DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
                "stream", reinterpret_cast<void *>(stream));
            sel = selReadStream(stream->f);
        }
    }

    if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        height = ll_opt_l_int32(_fun, L, 1, height);
        width = ll_opt_l_int32(_fun, L, 2, width);
        name = ll_check_string(_fun, L, 3);
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = '%s'\n", _fun,
            "height", height,
            "width", width,
            "name", name);
        sel = selCreate(height, width, name);
    }

    if (!sel && lua_isstring(L, 1)) {
        const char* fname = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "fname", fname);
        sel = selRead(fname);
    }

    if (!sel && lua_isstring(L, 1)) {
        const char* text = ll_check_string(_fun, L, 1);
        height = ll_opt_l_int32(_fun, L, 2, height);
        width = ll_opt_l_int32(_fun, L, 3, width);
        name = ll_check_string(_fun, L, 4);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s',  %s = %d, %s = %d, %s = '%s'\n", _fun,
            "text", text,
            "height", height,
            "width", width,
            "name", name);
        sel = selCreateFromString(text, height, width, name);
    }

    if (!sel) {
        DBG(LOG_NEW_PARAM, "%s: create for %s = %d, %s = %d, %s = '%s'\n", _fun,
            "height", height,
            "width", width,
            "name", name);
        sel = selCreate(height, width, name);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(sel));
    return ll_push_Sel(_fun, L, sel);
}
/**
 * \brief Register the PTA methods and functions in the Sel* meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
luaopen_Sel(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_Sel},    /* Sel() */
        {"__tostring",              toString},
        {"Copy",                    Copy},
        {"Create",                  Create},
        {"CreateBrick",             CreateBrick},
        {"CreateComb",              CreateComb},
        {"CreateFromColorPix",      CreateFromColorPix},
        {"CreateFromPix",           CreateFromPix},
        {"CreateFromPta",           CreateFromPta},
        {"CreateFromString",        CreateFromString},
        {"Destroy",                 Destroy},
        {"DisplayInPix",            DisplayInPix},
        {"FindMaxTranslations",     FindMaxTranslations},
        {"GetElement",              GetElement},
        {"GetName",                 GetName},
        {"GetParameters",           GetParameters},
        {"GetTypeAtOrigin",         GetTypeAtOrigin},
        {"PrintToString",           PrintToString},
        {"Read",                    Read},
        {"ReadMem",                 ReadFromColorImage},
        {"ReadStream",              ReadStream},
        {"RotateOrth",              RotateOrth},
        {"SelectComposableSels",    SelectComposableSels},
        {"SelectComposableSizes",   SelectComposableSizes},
        {"SetElement",              SetElement},
        {"SetName",                 SetName},
        {"SetOrigin",               SetOrigin},
        {"Write",                   Write},
        {"WriteStream",             WriteStream},
        LUA_SENTINEL
    };

    FUNC("luaopen_" TNAME);

    ll_global_cfunct(_fun, L, TNAME, ll_new_Sel);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
