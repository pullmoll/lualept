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
 * \file llccborda.cpp
 * \class CCBorda
 *
 * An array of CCBord.
 */

/** Set TNAME to the class name used in this source file */
#define TNAME LL_CCBORDA

/** Define a function's name (_fun) with prefix CCBorda */
#define LL_FUNC(x) FUNC(TNAME "." x)

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
    CCBorda **pccba = ll_check_udata<CCBorda>(_fun, L, 1, TNAME);
    CCBorda *ccba = *pccba;
    DBG(LOG_DESTROY, "%s: '%s' %s = %p, %s = %p, %s = %d\n", _fun,
        TNAME,
        "pboxa", reinterpret_cast<void *>(pccba),
        "boxa", reinterpret_cast<void *>(ccba),
        "count", ccbaGetCount(ccba));
    ccbaDestroy(&ccba);
    *pccba = nullptr;
    return 0;
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
 * \brief Display the CCBora* (%ccba) in a Pix* (%pix).
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 *
 * Leptonica's Notes:
 *      (1) Uses global ptaa, which gives each border pixel in
 *          global coordinates, and must be computed in advance
 *          by calling ccbaGenerateGlobalLocs().
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
 *
 * Leptonica's Notes:
 *      (1) Uses local ptaa, which gives each border pixel in
 *          local coordinates, so the actual pixel positions must
 *          be computed using all offsets.
 *      (2) For the holes, use coordinates relative to the c.c.
 *      (3) This is slower than Method 2.
 *      (4) This uses topological properties (Method 1) to do scan
 *          conversion to raster
 *
 *  This algorithm deserves some commentary.
 *
 *  I first tried the following:
 *    ~ outer borders: 4-fill from outside, stopping at the
 *         border, using pixFillClosedBorders()
 *    ~ inner borders: 4-fill from outside, stopping again
 *         at the border, XOR with the border, and invert
 *         to get the hole.  This did not work, because if
 *         you have a hole border that looks like:
 *
 *                x x x x x x
 *                x          x
 *                x   x x x   x
 *                  x x o x   x
 *                      x     x
 *                      x     x
 *                        x x x
 *
 *         if you 4-fill from the outside, the pixel 'o' will
 *         not be filled!  XORing with the border leaves it OFF.
 *         Inverting then gives a single bad ON pixel that is not
 *         actually part of the hole.
 *
 *  So what you must do instead is 4-fill the holes from inside.
 *  You can do this from a seedfill, using a pix with the hole
 *  border as the filling mask.  But you need to start with a
 *  pixel inside the hole.  How is this determined?  The best
 *  way is from the contour.  We have a right-hand shoulder
 *  rule for inside (i.e., the filled region).   Take the
 *  first 2 pixels of the hole border, and compute dx and dy
 *  (second coord minus first coord:  dx = sx - fx, dy = sy - fy).
 *  There are 8 possibilities, depending on the values of dx and
 *  dy (which can each be -1, 0, and +1, but not both 0).
 *  These 8 cases can be broken into 4; see the simple algorithm below.
 *  Once you have an interior seed pixel, you fill from the seed,
 *  clipping with the hole border pix by filling into its invert.
 *
 *  You then successively XOR these interior filled components, in any order.
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
 *
 * Leptonica's Notes:
 *      (1) Uses local chain ptaa, which gives each border pixel in
 *          local coordinates, so the actual pixel positions must
 *          be computed using all offsets.
 *      (2) Treats exterior and hole borders on equivalent
 *          footing, and does all calculations on a pix
 *          that spans the c.c. with a 1 pixel added boundary.
 *      (3) This uses topological properties (Method 2) to do scan
 *          conversion to raster
 *      (4) The algorithm is described at the top of this file (Method 2).
 *          It is preferred to Method 1 because it is between 1.2x and 2x
 *          faster than Method 1.
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
 *
 * Leptonica's Notes:
 *      (1) Uses spglobal pta, which gives each border pixel in
 *          global coordinates, one path per c.c., and must
 *          be computed in advance by calling ccbaGenerateSPGlobalLocs().
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
 *
 * Leptonica's Notes:
 *      (1) This calculates the splocal rep if not yet made.
 *      (2) It uses the local pixel values in splocal, the single
 *          path pta, which are all relative to each c.c., to find
 *          the corresponding global pixel locations, and stores
 *          them in the spglobal pta.
 *      (3) This lists only the turning points: it both makes a
 *          valid svg file and is typically about half the size
 *          when all border points are listed.
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
 *
 * Leptonica's Notes:
 *      (1) Generates a single border in local pixel coordinates.
 *          For each c.c., if there is just an outer border, copy it.
 *          If there are also hole borders, for each hole border,
 *          determine the smallest horizontal or vertical
 *          distance from the border to the outside of the c.c.,
 *          and find a path through the c.c. for this cut.
 *          We do this in a way that guarantees a pixel from the
 *          hole border is the starting point of the path, and
 *          we must verify that the path intersects the outer
 *          border (if it intersects it, then it ends on it).
 *          One can imagine pathological cases, but they may not
 *          occur in images of text characters and un-textured
 *          line graphics.
 *      (2) Once it is verified that the path through the c.c.
 *          intersects both the hole and outer borders, we
 *          generate the full single path for all borders in the
 *          c.c.  Starting at the start point on the outer
 *          border, when we hit a line on a cut, we take
 *          the cut, do the hold border, and return on the cut
 *          to the outer border.  We compose a pta of the
 *          outer border pts that are on cut paths, and for
 *          every point on the outer border (as we go around),
 *          we check against this pta.  When we find a matching
 *          point in the pta, we do its cut path and hole border.
 *          The single path is saved in the ccb.
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
 *
 * Leptonica's Notes:
 *      (1) This uses the pixel locs in the local ptaa,
 *          which are all relative to each c.c., to find
 *          the step directions for successive pixels in
 *          the chain, and stores them in the step numaa.
 *      (2) To get the step direction, use
 *              1   2   3
 *              0   P   4
 *              7   6   5
 *          where P is the previous pixel at (px, py).  The step direction
 *          is the number (from 0 through 7) for each relative location
 *          of the current pixel at (cx, cy).  It is easily found by
 *          indexing into a 2-d 3x3 array (dirtab).
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
 * \brief Brief comment goes here.
 * <pre>
 * Arg #1 (i.e. self) is expected to be a CCBorda* (ccba).
 * Arg #2 is expected to be a l_int32 (index).
 *
 * Leptonica's Notes:
 *      (1) This returns a clone of the ccb; it must be destroyed
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
 *
 * Leptonica's Notes:
 *      (1) This uses the step chain data in each ccb to determine
 *          the pixel locations, either global or local,
 *          and stores them in the appropriate ptaa,
 *          either global or local.  For the latter, the
 *          pixel locations are relative to the c.c.
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
 * \brief Check Lua stack at index (%arg) for udata of class CCBorda.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBorda* contained in the user data
 */
CCBorda *
ll_check_CCBorda(const char *_fun, lua_State *L, int arg)
{
    return *ll_check_udata<CCBorda>(_fun, L, arg, TNAME);
}

/**
 * \brief Check Lua stack at index %arg for udata of class CCBorda* and take it.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param arg index where to find the user data (usually 1)
 * \return pointer to the CCBorda* contained in the user data
 */
CCBorda *
ll_take_CCBorda(const char *_fun, lua_State *L, int arg)
{
    CCBorda **pccba = ll_check_udata<CCBorda>(_fun, L, arg, TNAME);
    CCBorda *ccba = *pccba;
    *pccba = nullptr;
    return ccba;
}

/**
 * \brief Take a CCBorda* from a global variable %name.
 * \param _fun calling function's name
 * \param L pointer to the lua_State
 * \param name of the global variable
 * \return pointer to the Amap* contained in the user data
 */
CCBorda *
ll_global_CCBorda(const char *_fun, lua_State *L, const char *name)
{
    if (LUA_TUSERDATA != lua_getglobal(L, name))
        return nullptr;
    return ll_take_CCBorda(_fun, L, 1);
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
    if (!ll_isudata(_fun, L, arg, TNAME))
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
    return ll_push_udata(_fun, L, TNAME, cd);
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

    if (ll_isudata(_fun, L, 1, LL_PIX)) {
        Pix* pixs = ll_opt_Pix(_fun, L, 1);
        l_int32 n = ll_opt_l_int32(_fun, L, 2, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %d\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pixs),
            "n", n);
        ccba = ccbaCreate(pixs, n);
    }

    if (ll_isudata(_fun, L, 1, LUA_FILEHANDLE)) {
        luaL_Stream *stream = ll_check_stream(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p\n", _fun,
            LUA_FILEHANDLE, reinterpret_cast<void *>(stream));
        ccba = ccbaReadStream(stream->f);
    }

    if (!ccba && ll_isstring(_fun, L, 1)) {
        const char* filename = ll_check_string(_fun, L, 1);
        DBG(LOG_NEW_PARAM, "%s: create for %s = '%s'\n", _fun,
            "filename", filename);
        ccba = ccbaRead(filename);
    }

    if (!ccba) {
        /* FIXME: create data for no pix? */
        Pix* pix = nullptr;
        l_int32 n = 1;
        DBG(LOG_NEW_PARAM, "%s: create for %s* = %p, %s = %d\n", _fun,
            LL_PIX, reinterpret_cast<void *>(pix),
            "n", n);
        ccba = ccbaCreate(nullptr, n);
    }

    DBG(LOG_NEW_CLASS, "%s: created %s* %p\n", _fun,
        TNAME, reinterpret_cast<void *>(ccba));
    return ll_push_CCBorda(_fun, L, ccba);
}

/**
 * \brief Register the CCBorda methods and functions in the CCBorda meta table.
 * \param L pointer to the lua_State
 * \return 1 table on the Lua stack
 */
int
ll_open_CCBorda(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"__gc",                    Destroy},
        {"__new",                   ll_new_CCBorda},
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
    LO_FUNC(TNAME);
    ll_global_cfunct(_fun, L, TNAME, ll_new_CCBorda);
    ll_register_class(_fun, L, TNAME, methods);
    return 1;
}
