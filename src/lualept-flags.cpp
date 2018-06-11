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
 * \file lualept-flags.cpp
 * Convert between strings and Leptonica enumeration values in both directions.
 */

/**
 * \brief Push a string to the Lua stack listing the table of options.
 * \param L Lua state
 * \param tbl table of key/value pairs
 * \param len length of that table
 * \param msg message to prepend to the listing of options.
 * \return string with enumeration value names and their keys
 */
int
ll_list_tbl_options(const char* _fun, lua_State *L, const lept_enum *tbl, size_t len, const char *msg)
{
    char *str = ll_calloc<char>(_fun, L, LL_STRBUFF);
    luaL_Buffer B;
    size_t maxw = 0;
    l_int32 value = -1;
    size_t i;

    luaL_buffinit(L, &B);

    /* Add the error message, if any */
    if (nullptr != msg)
        luaL_addstring(&B, msg);

    /* Find maximum width of all enumeration values */
    for (i = 0; i < len; i++) {
        const lept_enum* p = &tbl[i];
        if (p->name && strlen(p->name) > maxw)
            maxw = strlen(p->name);
    }

    /* List the options */
    for (i = 0; i < len; i++) {
        const lept_enum* p = &tbl[i];
        if (p->value != value) {
            /* this is a new enumeration value */
            if (nullptr != msg || i > 0)
                luaL_addchar(&B, '\n');
            snprintf(str, LL_STRBUFF, "%-*s: ", static_cast<int>(maxw), p->name, p->value);
            luaL_addstring(&B, str);
            value = p->value;
        } else {
            /* this is another key name for the same enumeration value */
            luaL_addstring(&B, ", ");
        }
        /* list the key string */
        snprintf(str, LL_STRBUFF, "'%s' ", p->key);
        luaL_addstring(&B, str);
    }
    luaL_pushresult(&B);
    ll_free(str);
    return 1;
}

/**
 * \brief Return a const char* with the (first) key for a enumeration value.
 * \param value value to search for
 * \param tbl table of key/value pairs
 * \param len length of that table
 * \return pointer to string with the (first) key for that value
 */
const char*
ll_string_tbl(l_int32 value, const lept_enum *tbl, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++) {
        const lept_enum* p = &tbl[i];
        if (p->value == value)
            return p->key;
    }
    return "<undefined>";
}

/**
 * \brief Find a option %str in a lept_enum_t array %tbl of size %len.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \param tbl table of key/name/value tuples
 * \param len length of that table
 * \return value or def
 */
l_int32
ll_check_tbl(const char *_fun, lua_State *L, int arg, l_int32 def, const lept_enum *tbl, size_t len)
{
    char msg[256];
    size_t i;

    const char* str = lua_isstring(L, arg) ? lua_tostring(L, arg) : nullptr;
    if (!str)
        return def;

    for (i = 0; i < len; i++) {
        const lept_enum* p = &tbl[i];
        if (!ll_strcasecmp(str, p->key))
            return p->value;
    }

    snprintf(msg, sizeof(msg), "%s: Invalid option #%d '%s'\n"
                               "Enumeration options:",
             _fun, arg, str);
    ll_list_tbl_options(_fun, L, tbl, len, msg);
    lua_error(L);
    return def;    /* NOTREACHED */

}

#define TBL_ENTRY(key,ENUMVALUE) { key, #ENUMVALUE, ENUMVALUE }

/**
 * \brief Table of debug log flag names and enumeration values.
 */
static const lept_enum tbl_debug[] = {
    TBL_ENTRY("register",       LOG_REGISTER),
    TBL_ENTRY("new",            LOG_NEW_PARAM | LOG_NEW_CLASS),
    TBL_ENTRY("new-param",      LOG_NEW_PARAM),
    TBL_ENTRY("new-class",      LOG_NEW_CLASS),
    TBL_ENTRY("destroy",        LOG_DESTROY),
    TBL_ENTRY("take",           LOG_TAKE),
    TBL_ENTRY("boolean",        LOG_PUSH_BOOLEAN | LOG_CHECK_BOOLEAN),
    TBL_ENTRY("push-boolean",   LOG_PUSH_BOOLEAN),
    TBL_ENTRY("check-boolean",  LOG_CHECK_BOOLEAN),
    TBL_ENTRY("integer",        LOG_PUSH_INTEGER | LOG_CHECK_INTEGER),
    TBL_ENTRY("push-integer",   LOG_PUSH_INTEGER),
    TBL_ENTRY("check-integer",  LOG_CHECK_INTEGER),
    TBL_ENTRY("number",         LOG_PUSH_NUMBER | LOG_CHECK_NUMBER),
    TBL_ENTRY("push-number",    LOG_PUSH_NUMBER),
    TBL_ENTRY("check-number",   LOG_CHECK_NUMBER),
    TBL_ENTRY("string",         LOG_PUSH_STRING | LOG_CHECK_STRING),
    TBL_ENTRY("push-string",    LOG_PUSH_STRING),
    TBL_ENTRY("check-string",   LOG_CHECK_STRING),
    TBL_ENTRY("udata",          LOG_PUSH_UDATA | LOG_CHECK_UDATA),
    TBL_ENTRY("push-udata",     LOG_PUSH_UDATA),
    TBL_ENTRY("check-udata",    LOG_CHECK_UDATA),
    TBL_ENTRY("array",          LOG_PUSH_ARRAY | LOG_CHECK_ARRAY),
    TBL_ENTRY("push-array",     LOG_PUSH_ARRAY),
    TBL_ENTRY("check-array",    LOG_CHECK_ARRAY),
    TBL_ENTRY("sdl2",           LOG_SDL2)
};

/**
 * \brief Check for a debug flag as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_debug(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_debug, ARRAYSIZE(tbl_debug));
}

/**
 * \brief Return a string representing enabled debug flags.
 * \param flag debug enable flags
 * \return pointer to const string
 */
const char*
ll_string_debug(l_int32 flag)
{
    static char str[1024];
    char *dst = str;
    size_t i;

    for (i = 0; i < ARRAYSIZE(tbl_debug); i++) {
        const lept_enum *e = &tbl_debug[i];
        l_int32 bit = e->value;
        if (bit != (bit & flag))
            continue;
        flag &= ~bit;
        if (dst > str) {
            dst += snprintf(dst, sizeof(str) - (size_t(dst - str)), "|");
        }
        dst += snprintf(dst, sizeof(str) - (size_t(dst - str)), "%s", e->key);
    }
    return str;
}

/**
 * \brief Table of access/storage flag names and enumeration values.
 * <pre>
 * Access and storage flags.
 * L_NOCOPY      : do not copy the object; do not delete the ptr
 * L_INSERT      : stuff it in; do not copy or clone
 * L_COPY        : make/use a copy of the object
 * L_CLONE       : make/use clone (ref count) of the object
 * L_COPY_CLONE  : make a new array object (e.g., pixa) and fill the array with clones (e.g., pix)
 * </pre>
 */
static const lept_enum tbl_access_storage[] = {
    TBL_ENTRY("nocopy",         L_NOCOPY),      /* do not copy the object; do not delete the ptr */
    TBL_ENTRY("insert",         L_INSERT),      /* stuff it in; do not copy or clone */
    TBL_ENTRY("copy",           L_COPY),        /* make/use a copy of the object */
    TBL_ENTRY("clone",          L_CLONE),       /* make/use clone (ref count) of the object */
    TBL_ENTRY("copy-clone",     L_COPY_CLONE)   /* make a new array object (e.g., pixa) and fill the array with clones (e.g., pix) */
};

/**
 * \brief Check for an optional storage flag as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_access_storage(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_access_storage, ARRAYSIZE(tbl_access_storage));
}

/**
 * \brief Return the name for an access/storage flag value.
 * \param flag access/storage flag
 * \return pointer to const string
 */
const char*
ll_string_access_storage(l_int32 flag)
{
    return ll_string_tbl(flag, tbl_access_storage, ARRAYSIZE(tbl_access_storage));
}

/**
 * \brief Table of access/storage flag names and enumeration values.
 * <pre>
 * 16-bit conversion flags.
 * </pre>
 */
static const lept_enum tbl_more_less_clip[] = {
    TBL_ENTRY("ls-byte",        L_LS_BYTE),
    TBL_ENTRY("lsb",            L_LS_BYTE),
    TBL_ENTRY("l",              L_LS_BYTE),
    TBL_ENTRY("ms-byte",        L_MS_BYTE),
    TBL_ENTRY("msb",            L_MS_BYTE),
    TBL_ENTRY("m",              L_MS_BYTE),
    TBL_ENTRY("auto-byte",      L_AUTO_BYTE),
    TBL_ENTRY("auto",           L_AUTO_BYTE),
    TBL_ENTRY("a",              L_AUTO_BYTE),
    TBL_ENTRY("clip-to-ff",     L_CLIP_TO_FF),
    TBL_ENTRY("clip-ff",        L_CLIP_TO_FF),
    TBL_ENTRY("ff",             L_CLIP_TO_FF),
    TBL_ENTRY("ls-two-bytes",   L_LS_TWO_BYTES),
    TBL_ENTRY("ls-2-bytes",     L_LS_TWO_BYTES),
    TBL_ENTRY("ls2b",           L_LS_TWO_BYTES),
    TBL_ENTRY("l2",             L_LS_TWO_BYTES),
    TBL_ENTRY("ms-two-bytes",   L_MS_TWO_BYTES),
    TBL_ENTRY("ms-2-bytes",     L_MS_TWO_BYTES),
    TBL_ENTRY("ms2b",           L_MS_TWO_BYTES),
    TBL_ENTRY("m2",             L_MS_TWO_BYTES),
    TBL_ENTRY("clip-to-ffff",   L_CLIP_TO_FFFF),
    TBL_ENTRY("clip-ffff",      L_CLIP_TO_FFFF),
    TBL_ENTRY("ffff",           L_CLIP_TO_FFFF)
};

/**
 * \brief Check for a byte type as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_more_less_clip(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_more_less_clip, ARRAYSIZE(tbl_more_less_clip));
}

/**
 * \brief Return the name for byte type value.
 * \param flag access/storage flag
 * \return pointer to const string
 */
const char*
ll_string_more_less_clip(l_int32 flag)
{
    return ll_string_tbl(flag, tbl_more_less_clip, ARRAYSIZE(tbl_more_less_clip));
}

/**
 * \brief Table of PDF encoding format names and enumeration values.
 * <pre>
 * Pdf formatted encoding types.
 * </pre>
 */
static const lept_enum tbl_encoding[] = {
    TBL_ENTRY("default-encode", L_DEFAULT_ENCODE),
    TBL_ENTRY("default",        L_DEFAULT_ENCODE),
    TBL_ENTRY("jpeg-encode",    L_JPEG_ENCODE),
    TBL_ENTRY("jpeg",           L_JPEG_ENCODE),
    TBL_ENTRY("jpg",            L_JPEG_ENCODE),
    TBL_ENTRY("g4-encode",      L_G4_ENCODE),
    TBL_ENTRY("g4",             L_G4_ENCODE),
    TBL_ENTRY("flate-encode",   L_FLATE_ENCODE),
    TBL_ENTRY("flate",          L_FLATE_ENCODE),
    TBL_ENTRY("jp2k-encode",    L_JP2K_ENCODE),
    TBL_ENTRY("jp2k",           L_JP2K_ENCODE)
};

/**
 * \brief Check for an PDF encoding format name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_encoding(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_encoding, ARRAYSIZE(tbl_encoding));
}

/**
 * \brief Return the name for an input file format (IFF_*).
 * \param encoding encoding enumeration value
 * \return pointer to const string
 */
const char*
ll_string_encoding(l_int32 encoding)
{
    return ll_string_tbl(encoding, tbl_encoding, ARRAYSIZE(tbl_encoding));
}

/**
 * \brief Table of input file format names and enumeration values.
 * <pre>
 * The IFF_DEFAULT flag is used to write the file out in the
 * same (input) file format that the pix was read from.  If the pix
 * was not read from file, the input format field will be
 * IFF_UNKNOWN and the output file format will be chosen to
 * be compressed and lossless; namely, IFF_TIFF_G4 for d = 1
 * and IFF_PNG for everything else.
 *
 * In the future, new format types that have defined extensions
 * will be added before IFF_DEFAULT, and will be kept in sync with
 * the file format extensions in writefile.c.  The positions of
 * file formats before IFF_DEFAULT will remain invariant.
 * </pre>
 */
static const lept_enum tbl_input_format[] = {
    TBL_ENTRY("unknown",         IFF_UNKNOWN),
    TBL_ENTRY("bmp",             IFF_BMP),
    TBL_ENTRY("jpg",             IFF_JFIF_JPEG),
    TBL_ENTRY("jpeg",            IFF_JFIF_JPEG),
    TBL_ENTRY("jfif",            IFF_JFIF_JPEG),
    TBL_ENTRY("png",             IFF_PNG),
    TBL_ENTRY("tiff",            IFF_TIFF),
    TBL_ENTRY("tif",             IFF_TIFF),
    TBL_ENTRY("tiff-packbits",   IFF_TIFF_PACKBITS),
    TBL_ENTRY("packbits",        IFF_TIFF_PACKBITS),
    TBL_ENTRY("tiff-rle",        IFF_TIFF_RLE),
    TBL_ENTRY("rle",             IFF_TIFF_RLE),
    TBL_ENTRY("tiff-g3",         IFF_TIFF_G3),
    TBL_ENTRY("g3",              IFF_TIFF_G3),
    TBL_ENTRY("tiff-g4",         IFF_TIFF_G4),
    TBL_ENTRY("g4",              IFF_TIFF_G4),
    TBL_ENTRY("tiff-lzw",        IFF_TIFF_LZW),
    TBL_ENTRY("lzw",             IFF_TIFF_LZW),
    TBL_ENTRY("tiff-zip",        IFF_TIFF_ZIP),
    TBL_ENTRY("zip",             IFF_TIFF_ZIP),
    TBL_ENTRY("pnm",             IFF_PNM),
    TBL_ENTRY("pbm",             IFF_PNM),
    TBL_ENTRY("pgm",             IFF_PNM),
    TBL_ENTRY("ppm",             IFF_PNM),
    TBL_ENTRY("ps",              IFF_PS),
    TBL_ENTRY("gif",             IFF_GIF),
    TBL_ENTRY("jp2",             IFF_JP2),
    TBL_ENTRY("jpeg2k",          IFF_JP2),
    TBL_ENTRY("webp",            IFF_WEBP),
    TBL_ENTRY("lpdf",            IFF_LPDF),
    TBL_ENTRY("default",         IFF_DEFAULT),
    TBL_ENTRY("spix",            IFF_SPIX)
};

/**
 * \brief Check for an image format name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_input_format(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_input_format, ARRAYSIZE(tbl_input_format));
}

/**
 * \brief Return the name for an input file format (IFF_*).
 * \param format input file format value
 * \return pointer to const string
 */
const char*
ll_string_input_format(int format)
{
    return ll_string_tbl(format, tbl_input_format, ARRAYSIZE(tbl_input_format));
}

/**
 * \brief Table of key type names for AMAP and ASET.
 * <pre>
 * The three valid key types for red-black trees, maps and sets.
 * </pre>
 */
static const lept_enum tbl_keytype[] = {
    TBL_ENTRY("int",        L_INT_TYPE),
    TBL_ENTRY("uint",       L_UINT_TYPE),
    TBL_ENTRY("float",      L_FLOAT_TYPE)
};

/**
 * \brief Check for an L_AMAP keytype name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_keytype(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_keytype, ARRAYSIZE(tbl_keytype));
}

/**
 * \brief Return a string for the keytype of an AMAP/ASET.
 * \param type key type value
 * \return const string with the name
 */
const char*
ll_string_keytype(l_int32 type)
{
    return ll_string_tbl(type, tbl_keytype, ARRAYSIZE(tbl_keytype));
}

/**
 * \brief Table of choice names and enumeration values.
 * <pre>
 * Set selection flags.
 * </pre>
 */
static const lept_enum tbl_consecutive_skip_by[] = {
    TBL_ENTRY("choose-consecutive", L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("consecutive",        L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("cons",               L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("c",                  L_CHOOSE_CONSECUTIVE),
    TBL_ENTRY("choose-skip-by",     L_CHOOSE_SKIP_BY),
    TBL_ENTRY("skip-by",            L_CHOOSE_SKIP_BY),
    TBL_ENTRY("skip",               L_CHOOSE_SKIP_BY),
    TBL_ENTRY("s",                  L_CHOOSE_SKIP_BY)
};

/**
 * \brief Check for an choose name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_consecutive_skip_by(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_consecutive_skip_by, ARRAYSIZE(tbl_consecutive_skip_by));
}

/**
 * \brief Return a string for the choice between consecutive and skip_by.
 * \param choice consecutive/skip_by enumeration value
 * \return const string with the name
 */
const char*
ll_string_consecutive_skip_by(l_int32 choice)
{
    return ll_string_tbl(choice, tbl_consecutive_skip_by, ARRAYSIZE(tbl_consecutive_skip_by));
}

/**
 * \brief Table of text orientation names and enumeration values.
 * <pre>
 * Text orientation flags.
 * </pre>
 */
static const lept_enum tbl_text_orientation[] = {
    TBL_ENTRY("text-orient-unknown",    L_TEXT_ORIENT_UNKNOWN),
    TBL_ENTRY("orient-unknown",         L_TEXT_ORIENT_UNKNOWN),
    TBL_ENTRY("unknown",                L_TEXT_ORIENT_UNKNOWN),
    TBL_ENTRY("text-orient-up",         L_TEXT_ORIENT_UP),
    TBL_ENTRY("orient-up",              L_TEXT_ORIENT_UP),
    TBL_ENTRY("up",                     L_TEXT_ORIENT_UP),
    TBL_ENTRY("u",                      L_TEXT_ORIENT_UP),
    TBL_ENTRY("text-orient-left",       L_TEXT_ORIENT_LEFT),
    TBL_ENTRY("orient-left",            L_TEXT_ORIENT_LEFT),
    TBL_ENTRY("left",                   L_TEXT_ORIENT_LEFT),
    TBL_ENTRY("l",                      L_TEXT_ORIENT_LEFT),
    TBL_ENTRY("text-orient-down",       L_TEXT_ORIENT_DOWN),
    TBL_ENTRY("orient-down",            L_TEXT_ORIENT_DOWN),
    TBL_ENTRY("down",                   L_TEXT_ORIENT_DOWN),
    TBL_ENTRY("d",                      L_TEXT_ORIENT_DOWN),
    TBL_ENTRY("text-orient-right",      L_TEXT_ORIENT_RIGHT),
    TBL_ENTRY("orient-right",           L_TEXT_ORIENT_RIGHT),
    TBL_ENTRY("right",                  L_TEXT_ORIENT_RIGHT),
    TBL_ENTRY("r",                      L_TEXT_ORIENT_RIGHT)
};

/**
 * \brief Check for a text orientation name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_text_orientation(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_text_orientation, ARRAYSIZE(tbl_text_orientation));
}

/**
 * \brief Return a string for the text orientation value.
 * \param orientation enumeration value
 * \return const string with the name
 */
const char*
ll_string_text_orientation(l_int32 orientation)
{
    return ll_string_tbl(orientation, tbl_text_orientation, ARRAYSIZE(tbl_text_orientation));
}

/**
 * \brief Table of edge orientation names and enumeration values.
 * <pre>
 * Edge orientation flags.
 * </pre>
 */
static const lept_enum tbl_edge_orientation[] = {
    TBL_ENTRY("horizontal-edges",       L_HORIZONTAL_EDGES),
    TBL_ENTRY("horizontal",             L_HORIZONTAL_EDGES),
    TBL_ENTRY("horiz",                  L_HORIZONTAL_EDGES),
    TBL_ENTRY("h",                      L_HORIZONTAL_EDGES),
    TBL_ENTRY("vertical-edges",         L_VERTICAL_EDGES),
    TBL_ENTRY("vertical",               L_VERTICAL_EDGES),
    TBL_ENTRY("vert",                   L_VERTICAL_EDGES),
    TBL_ENTRY("v",                      L_VERTICAL_EDGES),
    TBL_ENTRY("all-edges",              L_ALL_EDGES),
    TBL_ENTRY("all",                    L_ALL_EDGES),
    TBL_ENTRY("a",                      L_ALL_EDGES)
};

/**
 * \brief Check for a edge orientation name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_edge_orientation(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_edge_orientation, ARRAYSIZE(tbl_edge_orientation));
}

/**
 * \brief Return a string for the edge orientation value.
 * \param orientation enumeration value
 * \return const string with the name
 */
const char*
ll_string_edge_orientation(l_int32 orientation)
{
    return ll_string_tbl(orientation, tbl_edge_orientation, ARRAYSIZE(tbl_edge_orientation));
}

/**
 * \brief Table of color component names and enumeration values.
 * <pre>
 *  Notes:
 *      (1) These are the byte indices for colors in 32 bpp images.
 *          They are used through the GET/SET_DATA_BYTE accessors.
 *          The 4th byte, typically known as the "alpha channel" and used
 *          for blending, is used to a small extent in leptonica.
 *      (2) Do not change these values!  If you redefine them, functions
 *          that have the shifts hardcoded for efficiency and conciseness
 *          (instead of using the constants below) will break.  These
 *          functions are labelled with "***"  next to their names at
 *          the top of the files in which they are defined.
 *      (3) The shifts to extract the red, green, blue and alpha components
 *          from a 32 bit pixel are defined here.
 * </pre>
 */
static const lept_enum tbl_component[] = {
    TBL_ENTRY("red",             COLOR_RED),
    TBL_ENTRY("r",               COLOR_RED),
    TBL_ENTRY("green",           COLOR_GREEN),
    TBL_ENTRY("grn",             COLOR_GREEN),
    TBL_ENTRY("g",               COLOR_GREEN),
    TBL_ENTRY("blue",            COLOR_BLUE),
    TBL_ENTRY("blu",             COLOR_BLUE),
    TBL_ENTRY("b",               COLOR_BLUE),
    TBL_ENTRY("alpha",           L_ALPHA_CHANNEL),
    TBL_ENTRY("a",               L_ALPHA_CHANNEL)
};

/**
 * \brief Check for an component name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_component(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_component, ARRAYSIZE(tbl_component));
}

/**
 * \brief Return a string for the color component name.
 * \param component color component value
 * \return const string with the name
 */
const char*
ll_string_component(l_int32 component)
{
    return ll_string_tbl(component, tbl_component, ARRAYSIZE(tbl_component));
}

/**
 * \brief Table of color compression names and enumeration values.
 * <pre>
 * Compression to use for PDF.
 * </pre>
 */
static const lept_enum tbl_compression[] = {
    TBL_ENTRY("default",        IFF_DEFAULT),
    TBL_ENTRY("def",            IFF_DEFAULT),
    TBL_ENTRY("d",              IFF_DEFAULT),
    TBL_ENTRY("tiff-g4",        IFF_TIFF_G4),
    TBL_ENTRY("g4",             IFF_TIFF_G4),
    TBL_ENTRY("png",            IFF_PNG),
    TBL_ENTRY("p",              IFF_PNG),
    TBL_ENTRY("jfif-jpeg",      IFF_JFIF_JPEG),
    TBL_ENTRY("jpeg",           IFF_JFIF_JPEG),
    TBL_ENTRY("jpg",            IFF_JFIF_JPEG),
    TBL_ENTRY("jfif",           IFF_JFIF_JPEG),
    TBL_ENTRY("j",              IFF_JFIF_JPEG)
};

/**
 * \brief Check for an compression name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_compression(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_compression, ARRAYSIZE(tbl_compression));
}

/**
 * \brief Return a string for the color compression name.
 * \param compression color compression value
 * \return const string with the name
 */
const char*
ll_string_compression(l_int32 compression)
{
    return ll_string_tbl(compression, tbl_compression, ARRAYSIZE(tbl_compression));
}

/**
 * \brief Table of choice min/max names and enumeration values.
 * <pre>
 * Min/max selection flags.
 * </pre>
 */
static const lept_enum tbl_choose_min_max[] = {
    TBL_ENTRY("choose-min",         L_CHOOSE_MIN),
    TBL_ENTRY("min",                L_CHOOSE_MIN),
    TBL_ENTRY("choose-max",         L_CHOOSE_MAX),
    TBL_ENTRY("max",                L_CHOOSE_MAX),
    TBL_ENTRY("choose-maxdiff",     L_CHOOSE_MAXDIFF),
    TBL_ENTRY("maxdiff",            L_CHOOSE_MAXDIFF),
    TBL_ENTRY("choose-min-boost",   L_CHOOSE_MIN_BOOST),
    TBL_ENTRY("min-boost",          L_CHOOSE_MIN_BOOST),
    TBL_ENTRY("minb",               L_CHOOSE_MIN_BOOST),
    TBL_ENTRY("choose-max-boost",   L_CHOOSE_MAX_BOOST),
    TBL_ENTRY("max-boost",          L_CHOOSE_MAX_BOOST),
    TBL_ENTRY("maxb",               L_CHOOSE_MAX_BOOST)
};

/**
 * \brief Check for a min/max name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_choose_min_max(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_choose_min_max, ARRAYSIZE(tbl_choose_min_max));
}

/**
 * \brief Return a string for the choice between min and max.
 * \param choice min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_choose_min_max(l_int32 choice)
{
    return ll_string_tbl(choice, tbl_choose_min_max, ARRAYSIZE(tbl_choose_min_max));
}

/**
 * \brief Table of white/black is max names and enumeration values.
 * <pre>
 * Flags for 8 bit and 16 bit pixel sums.
 * </pre>
 */
static const lept_enum tbl_what_is_max[] = {
    TBL_ENTRY("white-is-max",    L_WHITE_IS_MAX),
    TBL_ENTRY("white",           L_WHITE_IS_MAX),
    TBL_ENTRY("w",               L_WHITE_IS_MAX),
    TBL_ENTRY("black-is-max",    L_BLACK_IS_MAX),
    TBL_ENTRY("black",           L_BLACK_IS_MAX),
    TBL_ENTRY("b",               L_BLACK_IS_MAX)
};

/**
 * \brief Check for a white or black is max name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_what_is_max(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_what_is_max, ARRAYSIZE(tbl_what_is_max));
}

/**
 * \brief Return a string for the choice between min and max.
 * \param what white or black is max enumeration value
 * \return const string with the name
 */
const char*
ll_string_what_is_max(l_int32 what)
{
    return ll_string_tbl(what, tbl_what_is_max, ARRAYSIZE(tbl_what_is_max));
}

/**
 * \brief Table of get white/black val names and enumeration values.
 * <pre>
 * Flags for getting white or black value.
 * </pre>
 */
static const lept_enum tbl_getval[] = {
    TBL_ENTRY("get-white-val",  L_GET_WHITE_VAL),
    TBL_ENTRY("white-val",      L_GET_WHITE_VAL),
    TBL_ENTRY("white",          L_GET_WHITE_VAL),
    TBL_ENTRY("w",              L_GET_WHITE_VAL),
    TBL_ENTRY("get-black-val",  L_GET_BLACK_VAL),
    TBL_ENTRY("black-val",      L_GET_BLACK_VAL),
    TBL_ENTRY("black",          L_GET_BLACK_VAL),
    TBL_ENTRY("b",              L_GET_BLACK_VAL)
};

/**
 * \brief Check for a L_GET_XXXX_VAL name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_getval(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_getval, ARRAYSIZE(tbl_getval));
}

/**
 * \brief Return a string for the choice between min and max.
 * \param val white or black getval enumeration value
 * \return const string with the name
 */
const char*
ll_string_getval(l_int32 val)
{
    return ll_string_tbl(val, tbl_getval, ARRAYSIZE(tbl_getval));
}

/**
 * \brief Table of direction names and enumeration values.
 * <pre>
 * Line orientation flags.
 * </pre>
 */
static const lept_enum tbl_direction[] = {
    TBL_ENTRY("horizontal-line", L_HORIZONTAL_LINE),
    TBL_ENTRY("horizontal",      L_HORIZONTAL_LINE),
    TBL_ENTRY("horiz",           L_HORIZONTAL_LINE),
    TBL_ENTRY("h",               L_HORIZONTAL_LINE),
    TBL_ENTRY("vertical-line",   L_VERTICAL_LINE),
    TBL_ENTRY("vertical",        L_VERTICAL_LINE),
    TBL_ENTRY("vert",            L_VERTICAL_LINE),
    TBL_ENTRY("v",               L_VERTICAL_LINE)
};

/**
 * \brief Check for a L_XXX_LINE name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_direction(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_direction, ARRAYSIZE(tbl_direction));
}

/**
 * \brief Return a string for the direction name.
 * \param dir horizontal or vertical line direction enumeration value
 * \return const string with the name
 */
const char*
ll_string_direction(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_direction, ARRAYSIZE(tbl_direction));
}

/**
 * \brief Table of distance names and enumeration values.
 * <pre>
 * Line orientation flags.
 * </pre>
 */
static const lept_enum tbl_distance[] = {
    TBL_ENTRY("manhattan-distance", L_MANHATTAN_DISTANCE),
    TBL_ENTRY("manhattan",          L_MANHATTAN_DISTANCE),
    TBL_ENTRY("m",                  L_MANHATTAN_DISTANCE),
    TBL_ENTRY("l1",                 L_MANHATTAN_DISTANCE),
    TBL_ENTRY("euclidian-distance", L_EUCLIDEAN_DISTANCE),
    TBL_ENTRY("euclidian",          L_EUCLIDEAN_DISTANCE),
    TBL_ENTRY("e",                  L_EUCLIDEAN_DISTANCE),
    TBL_ENTRY("l2",                 L_EUCLIDEAN_DISTANCE)
};

/**
 * \brief Check for a L_XXX_DISTANCE name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_distance(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_distance, ARRAYSIZE(tbl_distance));
}

/**
 * \brief Return a string for the distance name.
 * \param dir horizontal or vertical line distance enumeration value
 * \return const string with the name
 */
const char*
ll_string_distance(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_distance, ARRAYSIZE(tbl_distance));
}

/**
 * \brief Table of set white/black names and enumeration values.
 * <pre>
 * Flags for setting to white or black.
 * </pre>
 */
static const lept_enum tbl_set_black_white[] = {
    TBL_ENTRY("set-white",      L_SET_WHITE),
    TBL_ENTRY("white",          L_SET_WHITE),
    TBL_ENTRY("w",              L_SET_WHITE),
    TBL_ENTRY("set-black",      L_SET_BLACK),
    TBL_ENTRY("black",          L_SET_BLACK),
    TBL_ENTRY("b",              L_SET_BLACK)
};

/**
 * \brief Check for a set white or black name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_set_black_white(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_set_black_white, ARRAYSIZE(tbl_set_black_white));
}

/**
 * \brief Return a string for the choice between setting black or white.
 * \param which set black or white enumeration value
 * \return const string with the name
 */
const char*
ll_string_set_black_white(l_int32 which)
{
    return ll_string_tbl(which, tbl_set_black_white, ARRAYSIZE(tbl_set_black_white));
}

/**
 * \brief Table of rasterop names and enumeration values.
 * <pre>
 * The following operation bit flags have been modified from
 * Sun's pixrect.h.
 *
 * The 'op' in 'rasterop' is represented by an integer
 * composed with Boolean functions using the set of five integers
 * given below.  The integers, and the op codes resulting from
 * boolean expressions on them, need only be in the range from 0 to 15.
 * The function is applied on a per-pixel basis.
 *
 * Examples: the op code representing ORing the src and dest
 * is computed using the bit OR, as PIX_SRC | PIX_DST;  the op
 * code representing XORing src and dest is found from
 * PIX_SRC ^ PIX_DST;  the op code representing ANDing src and dest
 * is found from PIX_SRC & PIX_DST.  Note that
 * PIX_NOT(PIX_CLR) = PIX_SET, and v.v., as they must be.
 *
 * We use the following set of definitions:
 *
 *      #define   PIX_SRC      0xc
 *      #define   PIX_DST      0xa
 *      #define   PIX_NOT(op)  (op) ^ 0xf
 *      #define   PIX_CLR      0x0
 *      #define   PIX_SET      0xf
 *
 * These definitions differ from Sun's, in that Sun left-shifted
 * each value by 1 pixel, and used the least significant bit as a
 * flag for the "pseudo-operation" of clipping.  We don't need
 * this bit, because it is both efficient and safe ALWAYS to clip
 * the rectangles to the src and dest images, which is what we do.
 * See the notes in rop.h on the general choice of these bit flags.
 *
 * [If for some reason you need compatibility with Sun's xview package,
 * you can adopt the original Sun definitions to avoid redefinition conflicts:
 *
 *      #define   PIX_SRC      (0xc << 1)
 *      #define   PIX_DST      (0xa << 1)
 *      #define   PIX_NOT(op)  ((op) ^ 0x1e)
 *      #define   PIX_CLR      (0x0 << 1)
 *      #define   PIX_SET      (0xf << 1)
 * ]
 *
 * We have, for reference, the following 16 unique op flags:
 *
 *      PIX_CLR                           0000             0x0
 *      PIX_SET                           1111             0xf
 *      PIX_SRC                           1100             0xc
 *      PIX_DST                           1010             0xa
 *      PIX_NOT(PIX_SRC)                  0011             0x3
 *      PIX_NOT(PIX_DST)                  0101             0x5
 *      PIX_SRC | PIX_DST                 1110             0xe
 *      PIX_SRC & PIX_DST                 1000             0x8
 *      PIX_SRC ^ PIX_DST                 0110             0x6
 *      PIX_NOT(PIX_SRC) | PIX_DST        1011             0xb
 *      PIX_NOT(PIX_SRC) & PIX_DST        0010             0x2
 *      PIX_SRC | PIX_NOT(PIX_DST)        1101             0xd
 *      PIX_SRC & PIX_NOT(PIX_DST)        0100             0x4
 *      PIX_NOT(PIX_SRC | PIX_DST)        0001             0x1
 *      PIX_NOT(PIX_SRC & PIX_DST)        0111             0x7
 *      PIX_NOT(PIX_SRC ^ PIX_DST)        1001             0x9
 *
 * </pre>
 */
static const lept_enum tbl_rasterop[] = {
    TBL_ENTRY("clr",            PIX_CLR),
    TBL_ENTRY("set",            PIX_SET),
    TBL_ENTRY("src",            PIX_SRC),
    TBL_ENTRY("dst",            PIX_DST),
    TBL_ENTRY("!src",           PIX_NOT(PIX_SRC)),
    TBL_ENTRY("!dst",           PIX_NOT(PIX_DST)),
    TBL_ENTRY("src|dst",        PIX_SRC | PIX_DST),
    TBL_ENTRY("paint",          PIX_SRC | PIX_DST),
    TBL_ENTRY("src&dst",        PIX_SRC & PIX_DST),
    TBL_ENTRY("mask",           PIX_SRC & PIX_DST),
    TBL_ENTRY("src^dst",        PIX_SRC ^ PIX_DST),
    TBL_ENTRY("xor",            PIX_SRC ^ PIX_DST),
    TBL_ENTRY("!src|dst",       PIX_NOT(PIX_SRC) | PIX_DST),
    TBL_ENTRY("!src&dst",       PIX_NOT(PIX_SRC) & PIX_DST),
    TBL_ENTRY("subtract",       PIX_NOT(PIX_SRC) & PIX_DST),
    TBL_ENTRY("sub",            PIX_NOT(PIX_SRC) & PIX_DST),
    TBL_ENTRY("src|!dst",       PIX_SRC | PIX_NOT(PIX_DST)),
    TBL_ENTRY("src&!dst",       PIX_SRC & PIX_NOT(PIX_DST)),
    TBL_ENTRY("!(src|dst)",     PIX_NOT(PIX_SRC | PIX_DST)),
    TBL_ENTRY("!(src&dst)",     PIX_NOT(PIX_SRC & PIX_DST)),
    TBL_ENTRY("!(src^dst)",     PIX_NOT(PIX_SRC ^ PIX_DST))
};

/**
 * \brief Check for a rasterop name as string.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_rasterop(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_rasterop, ARRAYSIZE(tbl_rasterop));
}

/**
 * \brief Return a string for the raster operation.
 * \param op enumeration value of the raster operation
 * \return const string with the name
 */
const char*
ll_string_rasterop(l_int32 op)
{
    return ll_string_tbl(op, tbl_rasterop, ARRAYSIZE(tbl_rasterop));
}

/**
 * \brief Table of JPEG reader hint names and enumeration values.
 * <pre>
 * Hinting bit flags in jpeg reader.
 * </pre>
 */
static const lept_enum tbl_hint[] = {
    TBL_ENTRY("none",               0),
    TBL_ENTRY("read-luminance",     L_JPEG_READ_LUMINANCE),
    TBL_ENTRY("luminance",          L_JPEG_READ_LUMINANCE),
    TBL_ENTRY("lum",                L_JPEG_READ_LUMINANCE),
    TBL_ENTRY("l",                  L_JPEG_READ_LUMINANCE),
    TBL_ENTRY("fail-on-bad-data",   L_JPEG_FAIL_ON_BAD_DATA),
    TBL_ENTRY("fail",               L_JPEG_FAIL_ON_BAD_DATA),
    TBL_ENTRY("f",                  L_JPEG_FAIL_ON_BAD_DATA)
};

/**
 * \brief Check for a JPEG reader hint name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_hint(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_hint, ARRAYSIZE(tbl_hint));
}

/**
 * \brief Return a string for the JPEG reader hint.
 * \param dir enumeration value of the search direction
 * \return const string with the name
 */
const char*
ll_string_hint(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_hint, ARRAYSIZE(tbl_hint));
}

/**
 * \brief Table of search direction names and enumeration values.
 * <pre>
 * Direction flags for grayscale morphology, granulometry,         *
 * composable Sels, convolution, etc.
 * </pre>
 */
static const lept_enum tbl_searchdir[] = {
    TBL_ENTRY("horizontal",          L_HORIZ),
    TBL_ENTRY("horiz",               L_HORIZ),
    TBL_ENTRY("h",                   L_HORIZ),
    TBL_ENTRY("vertical",            L_VERT),
    TBL_ENTRY("vert",                L_VERT),
    TBL_ENTRY("v",                   L_VERT),
    TBL_ENTRY("both-directions",     L_BOTH_DIRECTIONS),
    TBL_ENTRY("both",                L_BOTH_DIRECTIONS),
    TBL_ENTRY("b",                   L_BOTH_DIRECTIONS)
};

/**
 * \brief Check for a search direction name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_searchdir(const char *_fun, lua_State *L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_searchdir, ARRAYSIZE(tbl_searchdir));
}

/**
 * \brief Return a string for the search direction.
 * \param dir enumeration value of the search direction
 * \return const string with the name
 */
const char*
ll_string_searchir(l_int32 dir)
{
    return ll_string_tbl(dir, tbl_searchdir, ARRAYSIZE(tbl_searchdir));
}

/**
 * \brief Table of stats type names and enumeration values.
 * <pre>
 * Flags for data type converted from Numa.
 * </pre>
 */
static const lept_enum tbl_number_value[] = {
    TBL_ENTRY("integer-value",      L_INTEGER_VALUE),
    TBL_ENTRY("integer",            L_INTEGER_VALUE),
    TBL_ENTRY("int",                L_INTEGER_VALUE),
    TBL_ENTRY("i",                  L_INTEGER_VALUE),
    TBL_ENTRY("float-value",        L_FLOAT_VALUE),
    TBL_ENTRY("float",              L_FLOAT_VALUE),
    TBL_ENTRY("f",                  L_FLOAT_VALUE)
};

/**
 * \brief Check for a stats type name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_number_value(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_number_value, ARRAYSIZE(tbl_number_value));
}

/**
 * \brief Return a string for the stats type enumeration value.
 * \param type enumeration value of the stats type
 * \return const string with the name
 */
const char*
ll_string_number_value(l_int32 type)
{
    return ll_string_tbl(type, tbl_number_value, ARRAYSIZE(tbl_number_value));
}

/**
 * \brief Table of position names and enumeration values.
 * <pre>
 * Pdf multi image flags.
 * </pre>
 */
static const lept_enum tbl_position[] = {
    TBL_ENTRY("single-image",       0),
    TBL_ENTRY("single",             0),
    TBL_ENTRY("s",                  0),
    TBL_ENTRY("first-image",        L_FIRST_IMAGE),
    TBL_ENTRY("first",              L_FIRST_IMAGE),
    TBL_ENTRY("f",                  L_FIRST_IMAGE),
    TBL_ENTRY("next-image",         L_NEXT_IMAGE),
    TBL_ENTRY("next",               L_NEXT_IMAGE),
    TBL_ENTRY("n",                  L_NEXT_IMAGE),
    TBL_ENTRY("last-image",         L_LAST_IMAGE),
    TBL_ENTRY("last",               L_LAST_IMAGE),
    TBL_ENTRY("l",                  L_LAST_IMAGE)
};

/**
 * \brief Check for a position name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_position(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_position, ARRAYSIZE(tbl_position));
}

/**
 * \brief Return a string for the position enumeration value.
 * \param type enumeration value of the position
 * \return const string with the name
 */
const char*
ll_string_position(l_int32 type)
{
    return ll_string_tbl(type, tbl_position, ARRAYSIZE(tbl_position));
}

/**
 * \brief Table of statistical measures names and enumeration values.
 * <pre>
 * Statistical measures.
 * </pre>
 */
static const lept_enum tbl_stats_type[] = {
    TBL_ENTRY("mean-absval",        L_MEAN_ABSVAL),
    TBL_ENTRY("mean-abs",           L_MEAN_ABSVAL),
    TBL_ENTRY("mean",               L_MEAN_ABSVAL),
    TBL_ENTRY("median-val",         L_MEDIAN_VAL),
    TBL_ENTRY("median",             L_MEDIAN_VAL),
    TBL_ENTRY("med",                L_MEDIAN_VAL),
    TBL_ENTRY("mode-val",           L_MODE_VAL),
    TBL_ENTRY("mode-count",         L_MODE_COUNT),
    TBL_ENTRY("root-mean-square",   L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("rms",                L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("r",                  L_ROOT_MEAN_SQUARE),
    TBL_ENTRY("standard-deviation", L_STANDARD_DEVIATION),
    TBL_ENTRY("stddev",             L_STANDARD_DEVIATION),
    TBL_ENTRY("s",                  L_STANDARD_DEVIATION),
    TBL_ENTRY("variance",           L_VARIANCE),
    TBL_ENTRY("var",                L_VARIANCE),
    TBL_ENTRY("v",                  L_VARIANCE)
};

/**
 * \brief Check for a stats type name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_stats_type(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_stats_type, ARRAYSIZE(tbl_stats_type));
}

/**
 * \brief Return a string for the stats type enumeration value.
 * \param type enumeration value of the stats type
 * \return const string with the name
 */
const char*
ll_string_stats_type(l_int32 type)
{
    return ll_string_tbl(type, tbl_stats_type, ARRAYSIZE(tbl_stats_type));
}

/**
 * \brief Table of color component selection names and enumeration values.
 * <pre>
 * Color component selection flags.
 * </pre>
 */
static const lept_enum tbl_select_color[] = {
    TBL_ENTRY("red",                 L_SELECT_RED),
    TBL_ENTRY("r",                   L_SELECT_RED),
    TBL_ENTRY("green",               L_SELECT_GREEN),
    TBL_ENTRY("grn",                 L_SELECT_GREEN),
    TBL_ENTRY("g",                   L_SELECT_GREEN),
    TBL_ENTRY("blue",                L_SELECT_BLUE),
    TBL_ENTRY("blu",                 L_SELECT_BLUE),
    TBL_ENTRY("b",                   L_SELECT_BLUE),
    TBL_ENTRY("min",                 L_SELECT_MIN),
    TBL_ENTRY("max",                 L_SELECT_MAX),
    TBL_ENTRY("average",             L_SELECT_AVERAGE),
    TBL_ENTRY("avg",                 L_SELECT_AVERAGE),
    TBL_ENTRY("hue",                 L_SELECT_HUE),
    TBL_ENTRY("saturation",          L_SELECT_SATURATION),
    TBL_ENTRY("sat",                 L_SELECT_SATURATION)
};

/**
 * \brief Check for a color component selection.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_select_color(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_select_color, ARRAYSIZE(tbl_select_color));
}

/**
 * \brief Return a string for the color component selection enumeration value.
 * \param color selected color enumeration value
 * \return const string with the name
 */
const char*
ll_string_select_color(l_int32 color)
{
    return ll_string_tbl(color, tbl_select_color, ARRAYSIZE(tbl_select_color));
}

/**
 * \brief Table of select min/max names and enumeration values.
 * <pre>
 * Color component selection flags. Only "min" and "max" subset.
 * </pre>
 */
static const lept_enum tbl_select_minmax[] = {
    TBL_ENTRY("min",                 L_SELECT_MIN),
    TBL_ENTRY("max",                 L_SELECT_MAX)
};

/**
 * \brief Check for a select min or max name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_select_min_max(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_select_minmax, ARRAYSIZE(tbl_select_minmax));
}

/**
 * \brief Return a string for the select min or max enumeration value.
 * \param which select min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_select_min_max(l_int32 which)
{
    return ll_string_tbl(which, tbl_select_minmax, ARRAYSIZE(tbl_select_minmax));
}

/**
 * \brief Table of structuring element type names and enumeration values.
 * <pre>
 * Structuring element types.
 * </pre>
 */
static const lept_enum tbl_sel[] = {
    TBL_ENTRY("dont-care",          SEL_DONT_CARE),
    TBL_ENTRY("hit",                SEL_HIT),
    TBL_ENTRY("h",                  SEL_HIT),
    TBL_ENTRY("miss",               SEL_MISS),
    TBL_ENTRY("m",                  SEL_MISS),
    TBL_ENTRY("",                   SEL_DONT_CARE)
};

/**
 * \brief Check for a structuring element type name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_sel(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_sel, ARRAYSIZE(tbl_sel));
}

/**
 * \brief Return a string for the structuring element type enumeration value.
 * \param which select min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_sel(l_int32 which)
{
    return ll_string_tbl(which, tbl_sel, ARRAYSIZE(tbl_sel));
}

/**
 * \brief Table of location filter names and enumeration values.
 * <pre>
 * Location filter flags.
 * </pre>
 */
static const lept_enum tbl_select_size[] = {
    TBL_ENTRY("width",              L_SELECT_WIDTH),
    TBL_ENTRY("w",                  L_SELECT_WIDTH),
    TBL_ENTRY("height",             L_SELECT_HEIGHT),
    TBL_ENTRY("h",                  L_SELECT_HEIGHT),
    TBL_ENTRY("xval",               L_SELECT_XVAL),
    TBL_ENTRY("x",                  L_SELECT_XVAL),
    TBL_ENTRY("yval",               L_SELECT_YVAL),
    TBL_ENTRY("y",                  L_SELECT_YVAL),
    TBL_ENTRY("if-either",          L_SELECT_IF_EITHER),
    TBL_ENTRY("either",             L_SELECT_IF_EITHER),
    TBL_ENTRY("e",                  L_SELECT_IF_EITHER),
    TBL_ENTRY("if-both",            L_SELECT_IF_BOTH),
    TBL_ENTRY("both",               L_SELECT_IF_BOTH),
    TBL_ENTRY("b",                  L_SELECT_IF_BOTH)
};

/**
 * \brief Check for a location filter name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_select_size(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_select_size, ARRAYSIZE(tbl_select_size));
}

/**
 * \brief Return a string for the location filter enumeration value.
 * \param which select min or max enumeration value
 * \return const string with the name
 */
const char*
ll_string_select_size(l_int32 which)
{
    return ll_string_tbl(which, tbl_select_size, ARRAYSIZE(tbl_select_size));
}

/**
 * \brief Table of sort by names and enumeration values.
 * <pre>
 * Sort type flags.
 * </pre>
 */
static const lept_enum tbl_sort_by[] = {
    TBL_ENTRY("sort-by-x",              L_SORT_BY_X),
    TBL_ENTRY("x",                      L_SORT_BY_X),
    TBL_ENTRY("sort-by-y",              L_SORT_BY_Y),
    TBL_ENTRY("y",                      L_SORT_BY_Y),
    TBL_ENTRY("sort-by-right",          L_SORT_BY_RIGHT),
    TBL_ENTRY("right",                  L_SORT_BY_RIGHT),
    TBL_ENTRY("r",                      L_SORT_BY_RIGHT),
    TBL_ENTRY("sort-by-bot",            L_SORT_BY_BOT),
    TBL_ENTRY("bottom",                 L_SORT_BY_BOT),
    TBL_ENTRY("bot",                    L_SORT_BY_BOT),
    TBL_ENTRY("b",                      L_SORT_BY_BOT),
    TBL_ENTRY("sort-by-width",          L_SORT_BY_WIDTH),
    TBL_ENTRY("width",                  L_SORT_BY_WIDTH),
    TBL_ENTRY("w",                      L_SORT_BY_WIDTH),
    TBL_ENTRY("sort-by-height",         L_SORT_BY_HEIGHT),
    TBL_ENTRY("height",                 L_SORT_BY_HEIGHT),
    TBL_ENTRY("h",                      L_SORT_BY_HEIGHT),
    TBL_ENTRY("sort-by-min-dimension",  L_SORT_BY_MIN_DIMENSION),
    TBL_ENTRY("min-dimension",          L_SORT_BY_MIN_DIMENSION),
    TBL_ENTRY("min",                    L_SORT_BY_MIN_DIMENSION),
    TBL_ENTRY("sort-by-max-dimension",  L_SORT_BY_MAX_DIMENSION),
    TBL_ENTRY("max-dimension",          L_SORT_BY_MAX_DIMENSION),
    TBL_ENTRY("max",                    L_SORT_BY_MAX_DIMENSION),
    TBL_ENTRY("sort-by-perimeter",      L_SORT_BY_PERIMETER),
    TBL_ENTRY("perimeter",              L_SORT_BY_PERIMETER),
    TBL_ENTRY("perim",                  L_SORT_BY_PERIMETER),
    TBL_ENTRY("p",                      L_SORT_BY_PERIMETER),
    TBL_ENTRY("sort-by-area",           L_SORT_BY_AREA),
    TBL_ENTRY("area",                   L_SORT_BY_AREA),
    TBL_ENTRY("a",                      L_SORT_BY_AREA),
    TBL_ENTRY("sort-by-aspect-ratio",   L_SORT_BY_ASPECT_RATIO),
    TBL_ENTRY("aspect-ratio",           L_SORT_BY_ASPECT_RATIO),
    TBL_ENTRY("aspect",                 L_SORT_BY_ASPECT_RATIO),
};

/**
 * \brief Check for a sort type name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_sort_by(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_sort_by, ARRAYSIZE(tbl_sort_by));
}

/**
 * \brief Return a string for sort type enumeration value.
 * \param sort_by selected sort by enumeration value
 * \return const string with the name
 */
const char*
ll_string_sort_by(l_int32 sort_by)
{
    return ll_string_tbl(sort_by, tbl_sort_by, ARRAYSIZE(tbl_sort_by));
}

/**
 * \brief Table of set side names and enumeration values.
 * <pre>
 * Box size adjustment and location flags. Only the "set-" subset.
 * </pre>
 */
static const lept_enum tbl_set_side[] = {
    TBL_ENTRY("set-left",           L_SET_LEFT),
    TBL_ENTRY("left",               L_SET_LEFT),
    TBL_ENTRY("lft",                L_SET_LEFT),
    TBL_ENTRY("l",                  L_SET_LEFT),
    TBL_ENTRY("set-right",          L_SET_RIGHT),
    TBL_ENTRY("right",              L_SET_RIGHT),
    TBL_ENTRY("rgt",                L_SET_RIGHT),
    TBL_ENTRY("r",                  L_SET_RIGHT),
    TBL_ENTRY("set-top",            L_SET_TOP),
    TBL_ENTRY("top",                L_SET_TOP),
    TBL_ENTRY("t",                  L_SET_TOP),
    TBL_ENTRY("set-bot",            L_SET_BOT),
    TBL_ENTRY("bottom",             L_SET_BOT),
    TBL_ENTRY("bot",                L_SET_BOT),
    TBL_ENTRY("b",                  L_SET_BOT)
};

/**
 * \brief Check for a from side name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_set_side(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_set_side, ARRAYSIZE(tbl_set_side));
}

/**
 * \brief Return a string for the from side enumeration value.
 * \param which from side enumeration value
 * \return const string with the name
 */
const char*
ll_string_set_side(l_int32 which)
{
    return ll_string_tbl(which, tbl_set_side, ARRAYSIZE(tbl_set_side));
}

/**
 * \brief Table of scan direction names and enumeration values.
 * <pre>
 * Scan direction flags. Only the "from-" subset.
 * </pre>
 */
static const lept_enum tbl_from_side[] = {
    TBL_ENTRY("from-left",      L_FROM_LEFT),
    TBL_ENTRY("left",           L_FROM_LEFT),
    TBL_ENTRY("lft",            L_FROM_LEFT),
    TBL_ENTRY("l",              L_FROM_LEFT),
    TBL_ENTRY("from-right",     L_FROM_RIGHT),
    TBL_ENTRY("right",          L_FROM_RIGHT),
    TBL_ENTRY("rgt",            L_FROM_RIGHT),
    TBL_ENTRY("r",              L_FROM_RIGHT),
    TBL_ENTRY("from-top",       L_FROM_TOP),
    TBL_ENTRY("top",            L_FROM_TOP),
    TBL_ENTRY("t",              L_FROM_TOP),
    TBL_ENTRY("from-bot",       L_FROM_BOT),
    TBL_ENTRY("bottom",         L_FROM_BOT),
    TBL_ENTRY("bot",            L_FROM_BOT),
    TBL_ENTRY("b",              L_FROM_BOT)
};

/**
 * \brief Check for a scan direction name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_from_side(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_from_side, ARRAYSIZE(tbl_from_side));
}

/**
 * \brief Return a string for the scan direction enumeration value.
 * \param which from side enumeration value
 * \return const string with the name
 */
const char*
ll_string_from_side(l_int32 which)
{
    return ll_string_tbl(which, tbl_from_side, ARRAYSIZE(tbl_from_side));
}

/**
 * \brief Table of adjust side names and enumeration values.
 * <pre>
 * Box size adjustment and location flags.
 * </pre>
 */
static const lept_enum tbl_adjust_sides[] = {
    TBL_ENTRY("adjust-skip",            L_ADJUST_SKIP),
    TBL_ENTRY("adj-skip",               L_ADJUST_SKIP),
    TBL_ENTRY("skip",                   L_ADJUST_SKIP),
    TBL_ENTRY("adjust-left",            L_ADJUST_LEFT),
    TBL_ENTRY("adj-left",               L_ADJUST_LEFT),
    TBL_ENTRY("left",                   L_ADJUST_LEFT),
    TBL_ENTRY("lft",                    L_ADJUST_LEFT),
    TBL_ENTRY("l",                      L_ADJUST_LEFT),
    TBL_ENTRY("adjust-right",           L_ADJUST_RIGHT),
    TBL_ENTRY("adj-right",              L_ADJUST_RIGHT),
    TBL_ENTRY("right",                  L_ADJUST_RIGHT),
    TBL_ENTRY("rgt",                    L_ADJUST_RIGHT),
    TBL_ENTRY("r",                      L_ADJUST_RIGHT),
    TBL_ENTRY("adjust-left-and-right",  L_ADJUST_LEFT_AND_RIGHT),
    TBL_ENTRY("adj-left-and-right",     L_ADJUST_LEFT_AND_RIGHT),
    TBL_ENTRY("left-and-right",         L_ADJUST_LEFT_AND_RIGHT),
    TBL_ENTRY("l-r",                    L_ADJUST_LEFT_AND_RIGHT),
    TBL_ENTRY("adjust-top",             L_ADJUST_TOP),
    TBL_ENTRY("adj-top",                L_ADJUST_TOP),
    TBL_ENTRY("top",                    L_ADJUST_TOP),
    TBL_ENTRY("t",                      L_ADJUST_TOP),
    TBL_ENTRY("adjust-bot",             L_ADJUST_BOT),
    TBL_ENTRY("adj-bot",                L_ADJUST_BOT),
    TBL_ENTRY("adjust-bottom",          L_ADJUST_BOT),
    TBL_ENTRY("bottom",                 L_ADJUST_BOT),
    TBL_ENTRY("bot",                    L_ADJUST_BOT),
    TBL_ENTRY("b",                      L_ADJUST_BOT),
    TBL_ENTRY("adjust-top-and-bot",     L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("adj-top-and-bot",        L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("top-and-bot",            L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("adjust-top-and-bottom",  L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("adj-top-and-bottom",     L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("top-and-bottom",         L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("t-b",                    L_ADJUST_TOP_AND_BOT),
    TBL_ENTRY("adjust-choose-min",      L_ADJUST_CHOOSE_MIN),
    TBL_ENTRY("adj-choose-min",         L_ADJUST_CHOOSE_MIN),
    TBL_ENTRY("choose-min",             L_ADJUST_CHOOSE_MIN),
    TBL_ENTRY("adjust-choose-max",      L_ADJUST_CHOOSE_MAX),
    TBL_ENTRY("adj-choose-max",         L_ADJUST_CHOOSE_MAX),
    TBL_ENTRY("choose-max",             L_ADJUST_CHOOSE_MAX),
    TBL_ENTRY("set-left",               L_SET_LEFT),
    TBL_ENTRY("set-l",                  L_SET_LEFT),
    TBL_ENTRY("set-right",              L_SET_RIGHT),
    TBL_ENTRY("set-r",                  L_SET_RIGHT),
    TBL_ENTRY("set-top",                L_SET_TOP),
    TBL_ENTRY("set-t",                  L_SET_TOP),
    TBL_ENTRY("set-bot",                L_SET_BOT),
    TBL_ENTRY("set-b",                  L_SET_BOT),
    TBL_ENTRY("get-left",               L_GET_LEFT),
    TBL_ENTRY("get-l",                  L_GET_LEFT),
    TBL_ENTRY("get-right",              L_GET_RIGHT),
    TBL_ENTRY("get-r",                  L_GET_RIGHT),
    TBL_ENTRY("get-top",                L_GET_TOP),
    TBL_ENTRY("get-t",                  L_GET_TOP),
    TBL_ENTRY("get-bot",                L_GET_BOT),
    TBL_ENTRY("get-b",                  L_GET_BOT)
};

/**
 * \brief Check for a adjust side name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_adjust_sides(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_adjust_sides, ARRAYSIZE(tbl_adjust_sides));
}

/**
 * \brief Return a string for the from adjust side enumeration value.
 * \param which from side enumeration value
 * \return const string with the name
 */
const char*
ll_string_adjust_sides(l_int32 which)
{
    return ll_string_tbl(which, tbl_adjust_sides, ARRAYSIZE(tbl_adjust_sides));
}

/**
 * \brief Table of sort mode by names and enumeration values.
 * <pre>
 * Sort mode flags.
 * </pre>
 */
static const lept_enum tbl_sort_mode[] = {
    TBL_ENTRY("shell-sort",             L_SHELL_SORT),
    TBL_ENTRY("shell",                  L_SHELL_SORT),
    TBL_ENTRY("s",                      L_SHELL_SORT),
    TBL_ENTRY("bin-sort",               L_BIN_SORT),
    TBL_ENTRY("bin",                    L_BIN_SORT),
    TBL_ENTRY("b",                      L_BIN_SORT)
};

/**
 * \brief Check for a sort mode name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_sort_mode(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_sort_mode, ARRAYSIZE(tbl_sort_mode));
}

/**
 * \brief Return a string for the sort mode.
 * \param sort_mode transform sort_mode enumeration value
 * \return const string with the name
 */
const char*
ll_string_sort_mode(l_int32 sort_mode)
{
    return ll_string_tbl(sort_mode, tbl_sort_mode, ARRAYSIZE(tbl_sort_mode));
}

/**
 * \brief Table of sort order by names and enumeration values.
 * <pre>
 * Sort order flags.
 * </pre>
 */
static const lept_enum tbl_sort_order[] = {
    TBL_ENTRY("increasing",             L_SORT_INCREASING),
    TBL_ENTRY("inc",                    L_SORT_INCREASING),
    TBL_ENTRY("i",                      L_SORT_INCREASING),
    TBL_ENTRY("decreasing",             L_SORT_DECREASING),
    TBL_ENTRY("dec",                    L_SORT_DECREASING),
    TBL_ENTRY("d",                      L_SORT_DECREASING)
};

/**
 * \brief Check for a sort order name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_sort_order(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_sort_order, ARRAYSIZE(tbl_sort_order));
}

/**
 * \brief Return a string for the sort order.
 * \param order transform sort_order enumeration value
 * \return const string with the name
 */
const char*
ll_string_sort_order(l_int32 order)
{
    return ll_string_tbl(order, tbl_sort_order, ARRAYSIZE(tbl_sort_order));
}

/**
 * \brief Table of transform order by names and enumeration values.
 */
static const lept_enum tbl_trans_order[] = {
    TBL_ENTRY("translate,scale,rotate", L_TR_SC_RO),
    TBL_ENTRY("tr,sc,ro",               L_TR_SC_RO),
    TBL_ENTRY("scale,rotate,translate", L_SC_RO_TR),
    TBL_ENTRY("sc,ro,tr",               L_SC_RO_TR),
    TBL_ENTRY("rotate,translate,scale", L_RO_TR_SC),
    TBL_ENTRY("ro,tr,sc",               L_RO_TR_SC),
    TBL_ENTRY("translate,rotate,scale", L_TR_RO_SC),
    TBL_ENTRY("tr,ro,sc",               L_TR_RO_SC),
    TBL_ENTRY("rotate,scale,translate", L_RO_SC_TR),
    TBL_ENTRY("ro,sc,tr",               L_RO_SC_TR),
    TBL_ENTRY("scale,translate,rotate", L_SC_TR_RO),
    TBL_ENTRY("sc,tr,ro",               L_SC_TR_RO)
};

/**
 * \brief Check for a transfort order name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_trans_order(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_trans_order, ARRAYSIZE(tbl_trans_order));
}

/**
 * \brief Return a string for the transform order.
 * \param order transform order enumeration value
 * \return const string with the name
 */
const char*
ll_string_trans_order(l_int32 order)
{
    return ll_string_tbl(order, tbl_trans_order, ARRAYSIZE(tbl_trans_order));
}

/**
 * \brief Table of include/exclude region by names and enumeration values.
 * <pre>
 * Size filter flags.
 * </pre>
 */
static const lept_enum tbl_region[] = {
    TBL_ENTRY("include-region",         L_INCLUDE_REGION),
    TBL_ENTRY("include",                L_INCLUDE_REGION),
    TBL_ENTRY("inc",                    L_INCLUDE_REGION),
    TBL_ENTRY("i",                      L_INCLUDE_REGION),
    TBL_ENTRY("exclude-region",         L_EXCLUDE_REGION),
    TBL_ENTRY("exclude",                L_EXCLUDE_REGION),
    TBL_ENTRY("exc",                    L_EXCLUDE_REGION),
    TBL_ENTRY("e",                      L_EXCLUDE_REGION)
};

/**
 * \brief Check for a include/exclude region name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_region(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_region, ARRAYSIZE(tbl_region));
}

/**
 * \brief Return a string for the include/exclude region.
 * \param region region value
 * \return const string with the name
 */
const char*
ll_string_region(l_int32 region)
{
    return ll_string_tbl(region, tbl_region, ARRAYSIZE(tbl_region));
}

/**
 * \brief Table of transform relation by names and enumeration values.
 * <pre>
 * Size filter flags.
 * </pre>
 */
static const lept_enum tbl_relation[] = {
    TBL_ENTRY("less-than",              L_SELECT_IF_LT),
    TBL_ENTRY("lt",                     L_SELECT_IF_LT),
    TBL_ENTRY("<",                      L_SELECT_IF_LT),
    TBL_ENTRY("greater-than",           L_SELECT_IF_GT),
    TBL_ENTRY("gt",                     L_SELECT_IF_GT),
    TBL_ENTRY(">",                      L_SELECT_IF_GT),
    TBL_ENTRY("less-than-or-equal",     L_SELECT_IF_LTE),
    TBL_ENTRY("lte",                    L_SELECT_IF_LTE),
    TBL_ENTRY("<=",                     L_SELECT_IF_LTE),
    TBL_ENTRY("greater-than-or-equal",  L_SELECT_IF_GTE),
    TBL_ENTRY("gte",                    L_SELECT_IF_GTE),
    TBL_ENTRY(">=",                     L_SELECT_IF_GTE)
};

/**
 * \brief Check for a transform relation name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_relation(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_relation, ARRAYSIZE(tbl_relation));
}

/**
 * \brief Return a string for the transform relation.
 * \param relation relation value
 * \return const string with the name
 */
const char*
ll_string_relation(l_int32 relation)
{
    return ll_string_tbl(relation, tbl_relation, ARRAYSIZE(tbl_relation));
}

/**
 * \brief Table of transform rotation by names and enumeration values.
 * <pre>
 * Translates degrees to clockwise count.
 * </pre>
 */
static const lept_enum tbl_rotation[] = {
    TBL_ENTRY("0",      0),
    TBL_ENTRY("90",     1),
    TBL_ENTRY("180",    2),
    TBL_ENTRY("270",    3),
    TBL_ENTRY("-90",    3),
    TBL_ENTRY("",       0)
};

/**
 * \brief Check for a rotation name (actually a number).
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_rotation(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_rotation, ARRAYSIZE(tbl_rotation));
}

/**
 * \brief Return a string for the rotation.
 * \param rotation rotation value
 * \return const string with the name
 */
const char*
ll_string_rotation(l_int32 rotation)
{
    return ll_string_tbl(rotation, tbl_rotation, ARRAYSIZE(tbl_rotation));
}

/**
 * \brief Table of handle overlap by names and enumeration values.
 * <pre>
 * Handling overlapping bounding boxes in Boxa.
 * </pre>
 */
static const lept_enum tbl_overlap[] = {
    TBL_ENTRY("combine",        L_COMBINE),
    TBL_ENTRY("comb",           L_COMBINE),
    TBL_ENTRY("c",              L_COMBINE),
    TBL_ENTRY("remove-small",   L_REMOVE_SMALL),
    TBL_ENTRY("remove",         L_REMOVE_SMALL),
    TBL_ENTRY("r",              L_REMOVE_SMALL)
};

/**
 * \brief Check for a handle overlap name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_overlap(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_overlap, ARRAYSIZE(tbl_overlap));
}

/**
 * \brief Return a string for the transform overlap.
 * \param overlap overlap value
 * \return const string with the name
 */
const char*
ll_string_overlap(l_int32 overlap)
{
    return ll_string_tbl(overlap, tbl_overlap, ARRAYSIZE(tbl_overlap));
}

/**
 * \brief Table of handle subflag by names and enumeration values.
 * <pre>
 * Flags for modifying box boundaries using a second box.
 * </pre>
 */
static const lept_enum tbl_subflag[] = {
    TBL_ENTRY("use-minsize",        L_USE_MINSIZE),
    TBL_ENTRY("minsize",            L_USE_MINSIZE),
    TBL_ENTRY("min",                L_USE_MINSIZE),
    TBL_ENTRY("use-maxsize",        L_USE_MAXSIZE),
    TBL_ENTRY("maxsize",            L_USE_MAXSIZE),
    TBL_ENTRY("max",                L_USE_MAXSIZE),
    TBL_ENTRY("sub-on-loc-diff",    L_SUB_ON_LOC_DIFF),
    TBL_ENTRY("loc-diff",           L_SUB_ON_LOC_DIFF),
    TBL_ENTRY("ldiff",              L_SUB_ON_LOC_DIFF),
    TBL_ENTRY("ld",                 L_SUB_ON_LOC_DIFF),
    TBL_ENTRY("sub-on-size-diff",   L_SUB_ON_SIZE_DIFF),
    TBL_ENTRY("size-diff",          L_SUB_ON_SIZE_DIFF),
    TBL_ENTRY("sdiff",              L_SUB_ON_SIZE_DIFF),
    TBL_ENTRY("sd",                 L_SUB_ON_SIZE_DIFF),
    TBL_ENTRY("use-capped-min",     L_USE_CAPPED_MIN),
    TBL_ENTRY("capped-min",         L_USE_CAPPED_MIN),
    TBL_ENTRY("capmin",             L_USE_CAPPED_MIN),
    TBL_ENTRY("cmin",               L_USE_CAPPED_MIN),
    TBL_ENTRY("use-capped-max",     L_USE_CAPPED_MAX),
    TBL_ENTRY("capped-max",         L_USE_CAPPED_MAX),
    TBL_ENTRY("capmax",             L_USE_CAPPED_MAX),
    TBL_ENTRY("cmax",               L_USE_CAPPED_MAX)
};

/**
 * \brief Check for a subflag name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_subflag(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_subflag, ARRAYSIZE(tbl_subflag));
}

/**
 * \brief Return a string for the subflag.
 * \param subflag subflag value
 * \return const string with the name
 */
const char*
ll_string_subflag(l_int32 subflag)
{
    return ll_string_tbl(subflag, tbl_subflag, ARRAYSIZE(tbl_subflag));
}

/**
 * \brief Table of handle use flag by names and enumeration values.
 * <pre>
 * Flags for replacing invalid boxes.
 * </pre>
 */
static const lept_enum tbl_useflag[] = {
    TBL_ENTRY("use-all-boxes",          L_USE_ALL_BOXES),
    TBL_ENTRY("use-all",                L_USE_ALL_BOXES),
    TBL_ENTRY("all",                    L_USE_ALL_BOXES),
    TBL_ENTRY("a",                      L_USE_ALL_BOXES),
    TBL_ENTRY("use-same-parity-boxes",  L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("use-same-parity",        L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("same-parity-boxes",      L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("same-parity",            L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("same",                   L_USE_SAME_PARITY_BOXES),
    TBL_ENTRY("s",                      L_USE_SAME_PARITY_BOXES)
};

/**
 * \brief Check for a use flag name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_useflag(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_useflag, ARRAYSIZE(tbl_useflag));
}

/**
 * \brief Return a string for the use flag.
 * \param useflag useflag value
 * \return const string with the name
 */
const char*
ll_string_useflag(l_int32 useflag)
{
    return ll_string_tbl(useflag, tbl_useflag, ARRAYSIZE(tbl_useflag));
}

/**
 * \brief Table of dist select by names and enumeration values.
 * <pre>
 * Handling negative values in conversion to unsigned int.
 * </pre>
 */
static const lept_enum tbl_negvals[] = {
    TBL_ENTRY("clip-to-zero",   L_CLIP_TO_ZERO),
    TBL_ENTRY("zero",           L_CLIP_TO_ZERO),
    TBL_ENTRY("z",              L_CLIP_TO_ZERO),
    TBL_ENTRY("take-absval",    L_TAKE_ABSVAL),
    TBL_ENTRY("absval",         L_TAKE_ABSVAL),
    TBL_ENTRY("abs",            L_TAKE_ABSVAL),
    TBL_ENTRY("a",              L_TAKE_ABSVAL)
};

/**
 * \brief Check for a negvals name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_negvals(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_negvals, ARRAYSIZE(tbl_negvals));
}

/**
 * \brief Return a string for the negvals enumeration value.
 * \param negvals enumeration value
 * \return const string with the name
 */
const char*
ll_string_negvals(l_int32 negvals)
{
    return ll_string_tbl(negvals, tbl_negvals, ARRAYSIZE(tbl_negvals));
}

/**
 * \brief Table of dist select by names and enumeration values.
 * <pre>
 * Value flags.
 * </pre>
 */
static const lept_enum tbl_value_flags[] = {
    TBL_ENTRY("negative",       L_NEGATIVE),
    TBL_ENTRY("neg",            L_NEGATIVE),
    TBL_ENTRY("n",              L_NEGATIVE),
    TBL_ENTRY("non-negative",   L_NON_NEGATIVE),
    TBL_ENTRY("non-neg",        L_NON_NEGATIVE),
    TBL_ENTRY("nn",             L_NON_NEGATIVE),
    TBL_ENTRY("positive",       L_POSITIVE),
    TBL_ENTRY("pos",            L_POSITIVE),
    TBL_ENTRY("p",              L_POSITIVE),
    TBL_ENTRY("non-positive",   L_NON_POSITIVE),
    TBL_ENTRY("non-pos",        L_NON_POSITIVE),
    TBL_ENTRY("np",             L_NON_POSITIVE),
    TBL_ENTRY("zero",           L_ZERO),
    TBL_ENTRY("z",              L_ZERO),
    TBL_ENTRY("all",            L_ALL),
    TBL_ENTRY("a",              L_ALL)
};

/**
 * \brief Check for a select min or max name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_value_flags(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_value_flags, ARRAYSIZE(tbl_value_flags));
}

/**
 * \brief Return a string for the transform value_flags.
 * \param value_flags value_flags value
 * \return const string with the name
 */
const char*
ll_string_value_flags(l_int32 value_flags)
{
    return ll_string_tbl(value_flags, tbl_value_flags, ARRAYSIZE(tbl_value_flags));
}

/**
 * \brief Table of paint type names and enumeration values.
 * <pre>
 * Paint flags.
 * </pre>
 */
static const lept_enum tbl_paint_flags[] = {
    TBL_ENTRY("paint-light",    L_PAINT_LIGHT),
    TBL_ENTRY("light",          L_PAINT_LIGHT),
    TBL_ENTRY("l",              L_PAINT_LIGHT),
    TBL_ENTRY("paint-dark",     L_PAINT_DARK),
    TBL_ENTRY("dark",           L_PAINT_DARK),
    TBL_ENTRY("d",              L_PAINT_DARK)
};

/**
 * \brief Check for a paint type name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_paint_flags(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_paint_flags, ARRAYSIZE(tbl_paint_flags));
}

/**
 * \brief Return a string for paint type enumeration value.
 * \param paint_type paint_type value
 * \return const string with the name
 */
const char*
ll_string_paint_flags(l_int32 paint_type)
{
    return ll_string_tbl(paint_type, tbl_paint_flags, ARRAYSIZE(tbl_paint_flags));
}

static const lept_enum tbl_color_name[] = {
    TBL_ENTRY("Black",                           0x000000),
    TBL_ENTRY("black",                           0x000000),
    TBL_ENTRY("Navy",                            0x000080),
    TBL_ENTRY("navy",                            0x000080),
    TBL_ENTRY("Dark Blue",                       0x00008b),
    TBL_ENTRY("darkblue",                        0x00008b),
    TBL_ENTRY("Medium Blue",                     0x0000cd),
    TBL_ENTRY("mediumblue",                      0x0000cd),
    TBL_ENTRY("Blue",                            0x0000ff),
    TBL_ENTRY("blue",                            0x0000ff),
    TBL_ENTRY("Dark Green",                      0x006400),
    TBL_ENTRY("darkgreen",                       0x006400),
    TBL_ENTRY("Green",                           0x008000),
    TBL_ENTRY("green",                           0x008000),
    TBL_ENTRY("Teal",                            0x008080),
    TBL_ENTRY("teal",                            0x008080),
    TBL_ENTRY("Dark Cyan",                       0x008b8b),
    TBL_ENTRY("darkcyan",                        0x008b8b),
    TBL_ENTRY("Deep Sky Blue",                   0x00bfff),
    TBL_ENTRY("deepskyblue",                     0x00bfff),
    TBL_ENTRY("Dark Turquoise",                  0x00ced1),
    TBL_ENTRY("darkturquoise",                   0x00ced1),
    TBL_ENTRY("Medium Spring Green",             0x00fa9a),
    TBL_ENTRY("mediumspringgreen",               0x00fa9a),
    TBL_ENTRY("Lime",                            0x00ff00),
    TBL_ENTRY("lime",                            0x00ff00),
    TBL_ENTRY("Spring Green",                    0x00ff7f),
    TBL_ENTRY("springgreen",                     0x00ff7f),
    TBL_ENTRY("Aqua",                            0x00ffff),
    TBL_ENTRY("aqua",                            0x00ffff),
    TBL_ENTRY("Cyan",                            0x00ffff),
    TBL_ENTRY("cyan",                            0x00ffff),
    TBL_ENTRY("Midnight Blue",                   0x191970),
    TBL_ENTRY("midnightblue",                    0x191970),
    TBL_ENTRY("Dodger Blue",                     0x1e90ff),
    TBL_ENTRY("dodgerblue",                      0x1e90ff),
    TBL_ENTRY("Light Sea Green",                 0x20b2aa),
    TBL_ENTRY("lightseagreen",                   0x20b2aa),
    TBL_ENTRY("Forest Green",                    0x228b22),
    TBL_ENTRY("forestgreen",                     0x228b22),
    TBL_ENTRY("Sea Green",                       0x2e8b57),
    TBL_ENTRY("seagreen",                        0x2e8b57),
    TBL_ENTRY("Dark Slate Gray",                 0x2f4f4f),
    TBL_ENTRY("darkslategray",                   0x2f4f4f),
    TBL_ENTRY("Lime Green",                      0x32cd32),
    TBL_ENTRY("limegreen",                       0x32cd32),
    TBL_ENTRY("Medium Sea Green",                0x3cb371),
    TBL_ENTRY("mediumseagreen",                  0x3cb371),
    TBL_ENTRY("Turquoise",                       0x40e0d0),
    TBL_ENTRY("turquoise",                       0x40e0d0),
    TBL_ENTRY("Royal Blue",                      0x4169e1),
    TBL_ENTRY("royalblue",                       0x4169e1),
    TBL_ENTRY("Steel Blue",                      0x4682b4),
    TBL_ENTRY("steelblue",                       0x4682b4),
    TBL_ENTRY("Dark Slate Blue",                 0x483d8b),
    TBL_ENTRY("darkslateblue",                   0x483d8b),
    TBL_ENTRY("Medium Turquoise",                0x48d1cc),
    TBL_ENTRY("mediumturquoise",                 0x48d1cc),
    TBL_ENTRY("Indigo",                          0x4b0082),
    TBL_ENTRY("indigo",                          0x4b0082),
    TBL_ENTRY("Dark Olive Green",                0x556b2f),
    TBL_ENTRY("darkolivegreen",                  0x556b2f),
    TBL_ENTRY("Cadet Blue",                      0x5f9ea0),
    TBL_ENTRY("cadetblue",                       0x5f9ea0),
    TBL_ENTRY("Cornflower Blue",                 0x6495ed),
    TBL_ENTRY("cornflowerblue",                  0x6495ed),
    TBL_ENTRY("Medium Aquamarine",               0x66cdaa),
    TBL_ENTRY("mediumaquamarine",                0x66cdaa),
    TBL_ENTRY("Dim Gray",                        0x696969),
    TBL_ENTRY("dimgray",                         0x696969),
    TBL_ENTRY("Slate Blue",                      0x6a5acd),
    TBL_ENTRY("slateblue",                       0x6a5acd),
    TBL_ENTRY("Olive Drab",                      0x6b8e23),
    TBL_ENTRY("olivedrab",                       0x6b8e23),
    TBL_ENTRY("Slate Gray",                      0x708090),
    TBL_ENTRY("slategray",                       0x708090),
    TBL_ENTRY("Light Slate Gray",                0x778899),
    TBL_ENTRY("lightslategray",                  0x778899),
    TBL_ENTRY("Medium Slate Blue",               0x7b68ee),
    TBL_ENTRY("mediumslateblue",                 0x7b68ee),
    TBL_ENTRY("Lawn Green",                      0x7cfc00),
    TBL_ENTRY("lawngreen",                       0x7cfc00),
    TBL_ENTRY("Chartreuse",                      0x7fff00),
    TBL_ENTRY("chartreuse",                      0x7fff00),
    TBL_ENTRY("Aquamarine",                      0x7fffd4),
    TBL_ENTRY("aquamarine",                      0x7fffd4),
    TBL_ENTRY("Maroon",                          0x800000),
    TBL_ENTRY("maroon",                          0x800000),
    TBL_ENTRY("Purple",                          0x800080),
    TBL_ENTRY("purple",                          0x800080),
    TBL_ENTRY("Olive",                           0x808000),
    TBL_ENTRY("olive",                           0x808000),
    TBL_ENTRY("Gray",                            0x808080),
    TBL_ENTRY("gray",                            0x808080),
    TBL_ENTRY("Sky Blue",                        0x87ceeb),
    TBL_ENTRY("skyblue",                         0x87ceeb),
    TBL_ENTRY("Light Sky Blue",                  0x87cefa),
    TBL_ENTRY("lightskyblue",                    0x87cefa),
    TBL_ENTRY("Blue Violet",                     0x8a2be2),
    TBL_ENTRY("blueviolet",                      0x8a2be2),
    TBL_ENTRY("Dark Red",                        0x8b0000),
    TBL_ENTRY("darkred",                         0x8b0000),
    TBL_ENTRY("Dark Magenta",                    0x8b008b),
    TBL_ENTRY("darkmagenta",                     0x8b008b),
    TBL_ENTRY("Saddle Brown",                    0x8b4513),
    TBL_ENTRY("saddlebrown",                     0x8b4513),
    TBL_ENTRY("Dark Sea Green",                  0x8fbc8f),
    TBL_ENTRY("darkseagreen",                    0x8fbc8f),
    TBL_ENTRY("Light Green",                     0x90ee90),
    TBL_ENTRY("lightgreen",                      0x90ee90),
    TBL_ENTRY("Medium Purple",                   0x9370db),
    TBL_ENTRY("mediumpurple",                    0x9370db),
    TBL_ENTRY("Dark Violet",                     0x9400d3),
    TBL_ENTRY("darkviolet",                      0x9400d3),
    TBL_ENTRY("Pale Green",                      0x98fb98),
    TBL_ENTRY("palegreen",                       0x98fb98),
    TBL_ENTRY("Dark Orchid",                     0x9932cc),
    TBL_ENTRY("darkorchid",                      0x9932cc),
    TBL_ENTRY("Yellow Green",                    0x9acd32),
    TBL_ENTRY("yellowgreen",                     0x9acd32),
    TBL_ENTRY("Sienna",                          0xa0522d),
    TBL_ENTRY("sienna",                          0xa0522d),
    TBL_ENTRY("Brown",                           0xa52a2a),
    TBL_ENTRY("brown",                           0xa52a2a),
    TBL_ENTRY("Dark Gray",                       0xa9a9a9),
    TBL_ENTRY("darkgray",                        0xa9a9a9),
    TBL_ENTRY("Light Blue",                      0xadd8e6),
    TBL_ENTRY("lightblue",                       0xadd8e6),
    TBL_ENTRY("Green Yellow",                    0xadff2f),
    TBL_ENTRY("greenyellow",                     0xadff2f),
    TBL_ENTRY("Pale Turquoise",                  0xafeeee),
    TBL_ENTRY("paleturquoise",                   0xafeeee),
    TBL_ENTRY("Light Steel Blue",                0xb0c4de),
    TBL_ENTRY("lightsteelblue",                  0xb0c4de),
    TBL_ENTRY("Powder Blue",                     0xb0e0e6),
    TBL_ENTRY("powderblue",                      0xb0e0e6),
    TBL_ENTRY("Fire Brick",                      0xb22222),
    TBL_ENTRY("firebrick",                       0xb22222),
    TBL_ENTRY("Dark Goldenrod",                  0xb8860b),
    TBL_ENTRY("darkgoldenrod",                   0xb8860b),
    TBL_ENTRY("Medium Orchid",                   0xba55d3),
    TBL_ENTRY("mediumorchid",                    0xba55d3),
    TBL_ENTRY("Rosy Brown",                      0xbc8f8f),
    TBL_ENTRY("rosybrown",                       0xbc8f8f),
    TBL_ENTRY("Dark Khaki",                      0xbdb76b),
    TBL_ENTRY("darkkhaki",                       0xbdb76b),
    TBL_ENTRY("Silver",                          0xc0c0c0),
    TBL_ENTRY("Medium Violet Red",               0xc71585),
    TBL_ENTRY("mediumvioletred",                 0xc71585),
    TBL_ENTRY("Indian Red",                      0xcd5c5c),
    TBL_ENTRY("indianred",                       0xcd5c5c),
    TBL_ENTRY("Peru",                            0xcd853f),
    TBL_ENTRY("peru",                            0xcd853f),
    TBL_ENTRY("Chocolate",                       0xd2691e),
    TBL_ENTRY("chocolate",                       0xd2691e),
    TBL_ENTRY("Tan",                             0xd2b48c),
    TBL_ENTRY("tan",                             0xd2b48c),
    TBL_ENTRY("Light Grey",                      0xd3d3d3),
    TBL_ENTRY("lightgrey",                       0xd3d3d3),
    TBL_ENTRY("Thistle",                         0xd8bfd8),
    TBL_ENTRY("thistle",                         0xd8bfd8),
    TBL_ENTRY("Orchid",                          0xda70d6),
    TBL_ENTRY("orchid",                          0xda70d6),
    TBL_ENTRY("Goldenrod",                       0xdaa520),
    TBL_ENTRY("goldenrod",                       0xdaa520),
    TBL_ENTRY("Pale Violet Red",                 0xdb7093),
    TBL_ENTRY("palevioletred",                   0xdb7093),
    TBL_ENTRY("Crimson",                         0xdc143c),
    TBL_ENTRY("crimson",                         0xdc143c),
    TBL_ENTRY("Gainsboro",                       0xdcdcdc),
    TBL_ENTRY("gainsboro",                       0xdcdcdc),
    TBL_ENTRY("Plum",                            0xdda0dd),
    TBL_ENTRY("plum",                            0xdda0dd),
    TBL_ENTRY("Burly Wood",                      0xdeb887),
    TBL_ENTRY("burlywood",                       0xdeb887),
    TBL_ENTRY("Light Cyan",                      0xe0ffff),
    TBL_ENTRY("lightcyan",                       0xe0ffff),
    TBL_ENTRY("Lavender",                        0xe6e6fa),
    TBL_ENTRY("lavender",                        0xe6e6fa),
    TBL_ENTRY("Dark Salmon",                     0xe9967a),
    TBL_ENTRY("darksalmon",                      0xe9967a),
    TBL_ENTRY("Violet",                          0xee82ee),
    TBL_ENTRY("violet",                          0xee82ee),
    TBL_ENTRY("Pale Goldenrod",                  0xeee8aa),
    TBL_ENTRY("palegoldenrod",                   0xeee8aa),
    TBL_ENTRY("Light Coral",                     0xf08080),
    TBL_ENTRY("lightcoral",                      0xf08080),
    TBL_ENTRY("Khaki",                           0xf0e68c),
    TBL_ENTRY("khaki",                           0xf0e68c),
    TBL_ENTRY("Alice Blue",                      0xf0f8ff),
    TBL_ENTRY("aliceblue",                       0xf0f8ff),
    TBL_ENTRY("Honeydew",                        0xf0fff0),
    TBL_ENTRY("honeydew",                        0xf0fff0),
    TBL_ENTRY("Azure",                           0xf0ffff),
    TBL_ENTRY("azure",                           0xf0ffff),
    TBL_ENTRY("Sandy Brown",                     0xf4a460),
    TBL_ENTRY("sandybrown",                      0xf4a460),
    TBL_ENTRY("Wheat",                           0xf5deb3),
    TBL_ENTRY("wheat",                           0xf5deb3),
    TBL_ENTRY("Beige",                           0xf5f5dc),
    TBL_ENTRY("beige",                           0xf5f5dc),
    TBL_ENTRY("White Smoke",                     0xf5f5f5),
    TBL_ENTRY("whitesmoke",                      0xf5f5f5),
    TBL_ENTRY("Mint Cream",                      0xf5fffa),
    TBL_ENTRY("mintcream",                       0xf5fffa),
    TBL_ENTRY("Ghost White",                     0xf8f8ff),
    TBL_ENTRY("ghostwhite",                      0xf8f8ff),
    TBL_ENTRY("Salmon",                          0xfa8072),
    TBL_ENTRY("salmon",                          0xfa8072),
    TBL_ENTRY("Antique White",                   0xfaebd7),
    TBL_ENTRY("antiquewhite",                    0xfaebd7),
    TBL_ENTRY("Linen",                           0xfaf0e6),
    TBL_ENTRY("linen",                           0xfaf0e6),
    TBL_ENTRY("Light Goldenrod Yellow",          0xfafad2),
    TBL_ENTRY("lightgoldenrodyellow",            0xfafad2),
    TBL_ENTRY("Old Lace",                        0xfdf5e6),
    TBL_ENTRY("oldlace",                         0xfdf5e6),
    TBL_ENTRY("Red",                             0xff0000),
    TBL_ENTRY("red",                             0xff0000),
    TBL_ENTRY("fuchsia",                         0xff00ff),
    TBL_ENTRY("Fuchsia",                         0xff00ff),
    TBL_ENTRY("Magenta",                         0xff00ff),
    TBL_ENTRY("magenta",                         0xff00ff),
    TBL_ENTRY("Deep Pink",                       0xff1493),
    TBL_ENTRY("deeppink",                        0xff1493),
    TBL_ENTRY("Orange Red",                      0xff4500),
    TBL_ENTRY("orangered",                       0xff4500),
    TBL_ENTRY("Tomato",                          0xff6347),
    TBL_ENTRY("tomato",                          0xff6347),
    TBL_ENTRY("Hot Pink",                        0xff69b4),
    TBL_ENTRY("hotpink",                         0xff69b4),
    TBL_ENTRY("Coral",                           0xff7f50),
    TBL_ENTRY("coral",                           0xff7f50),
    TBL_ENTRY("Dark Orange",                     0xff8c00),
    TBL_ENTRY("darkorange",                      0xff8c00),
    TBL_ENTRY("Light Salmon",                    0xffa07a),
    TBL_ENTRY("lightsalmon",                     0xffa07a),
    TBL_ENTRY("Orange",                          0xffa500),
    TBL_ENTRY("orange",                          0xffa500),
    TBL_ENTRY("Light Pink",                      0xffb6c1),
    TBL_ENTRY("lightpink",                       0xffb6c1),
    TBL_ENTRY("Pink",                            0xffc0cb),
    TBL_ENTRY("pink",                            0xffc0cb),
    TBL_ENTRY("Gold",                            0xffd700),
    TBL_ENTRY("gold",                            0xffd700),
    TBL_ENTRY("Peach Puff",                      0xffdab9),
    TBL_ENTRY("peachpuff",                       0xffdab9),
    TBL_ENTRY("Navajo White",                    0xffdead),
    TBL_ENTRY("navajowhite",                     0xffdead),
    TBL_ENTRY("Moccasin",                        0xffe4b5),
    TBL_ENTRY("moccasin",                        0xffe4b5),
    TBL_ENTRY("Bisque",                          0xffe4c4),
    TBL_ENTRY("bisque",                          0xffe4c4),
    TBL_ENTRY("Misty Rose",                      0xffe4e1),
    TBL_ENTRY("mistyrose",                       0xffe4e1),
    TBL_ENTRY("Blanched Almond",                 0xffebcd),
    TBL_ENTRY("blanchedalmond",                  0xffebcd),
    TBL_ENTRY("Papaya Whip",                     0xffefd5),
    TBL_ENTRY("papayawhip",                      0xffefd5),
    TBL_ENTRY("Lavender Blush",                  0xfff0f5),
    TBL_ENTRY("lavenderblush",                   0xfff0f5),
    TBL_ENTRY("Seashell",                        0xfff5ee),
    TBL_ENTRY("seashell",                        0xfff5ee),
    TBL_ENTRY("Cornsilk",                        0xfff8dc),
    TBL_ENTRY("cornsilk",                        0xfff8dc),
    TBL_ENTRY("Lemon Chiffon",                   0xfffacd),
    TBL_ENTRY("lemonchiffon",                    0xfffacd),
    TBL_ENTRY("Floral White",                    0xfffaf0),
    TBL_ENTRY("floralwhite",                     0xfffaf0),
    TBL_ENTRY("Snow",                            0xfffafa),
    TBL_ENTRY("snow",                            0xfffafa),
    TBL_ENTRY("Yellow",                          0xffff00),
    TBL_ENTRY("yellow",                          0xffff00),
    TBL_ENTRY("Light Yellow",                    0xffffe0),
    TBL_ENTRY("lightyellow",                     0xffffe0),
    TBL_ENTRY("Ivory",                           0xfffff0),
    TBL_ENTRY("ivory",                           0xfffff0),
    TBL_ENTRY("white",                           0xffffff),
    TBL_ENTRY("White",                           0xffffff),
};

/**
 * \brief Check for a color name.
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index where to find the string
 * \param def default value to return if not specified or unknown
 * \return storage flag
 */
l_int32
ll_check_color_name(const char *_fun, lua_State* L, int arg, l_int32 def)
{
    return ll_check_tbl(_fun, L, arg, def, tbl_color_name, ARRAYSIZE(tbl_color_name));
}

/**
 * \brief Return a string for color RGB value.
 * \param color color value
 * \return const string with the name
 */
const char*
ll_string_color_name(l_uint32 color)
{
    static char buff[64];
    l_int32 r, g, b;
    extractRGBValues(color, &r, &g, &b);
    l_uint32 value = static_cast<l_uint32>((r << 16) | (g << 8) | (b << 0));
    const char* name = ll_string_tbl(static_cast<l_int32>(value), tbl_color_name, ARRAYSIZE(tbl_color_name));
    if (strcmp(name, "<undefined>"))
        return name;
    snprintf(buff, sizeof(buff), "#%02X%02x%02X", r, g, b);
    return buff;
}

/**
 * \brief Let's try our best to convert argument(s) to RGBA values
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index of the first parameter
 * \param pr [optional] pointer to red value to return
 * \param pg [optional] pointer to green value to return
 * \param pb [optional] pointer to blue value to return
 * \param pa [optional] pointer to alpah value to return
 * \return 0 on success, or 1 on error
 */
int
ll_check_color(const char *_fun, lua_State *L, int arg, l_int32 *pr, l_int32 *pg, l_int32 *pb, l_int32 *pa)
{
    l_int32 color = 1 << 24;
    l_int32 r = 0;
    l_int32 g = 0;
    l_int32 b = 0;
    l_int32 a = 0;

    if (pr)
        *pr = 0;
    if (pg)
        *pg = 0;
    if (pb)
        *pb = 0;
    if (pa)
        *pa = 0;

    if (ll_istable(_fun, L, arg)) {
        /* expect a table with up to 4 integer values */
        l_int32 len = 0;
        l_int32 *tbl = ll_unpack_Iarray(_fun, L, arg, &len);
        if (len > 0)
            r = tbl[0];
        if (len > 1)
            g = tbl[1];
        if (len > 2)
            b = tbl[2];
        if (len > 3)
            b = tbl[3];
        ll_free(tbl);
    } else if (ll_isinteger(_fun, L, arg)) {
        /*
         * Expect min 1 to max 4 integer values, where g defaults to r
         * and b defaults to g, so that color(60) => 60, 60, 60
         * Alpha defaults to 255.
         */
        r = ll_check_l_int32(_fun, L, arg);
        g = ll_opt_l_int32(_fun, L, arg + 1, r);
        b = ll_opt_l_int32(_fun, L, arg + 2, g);
        a = ll_opt_l_int32(_fun, L, arg + 3, 255);
    } else if (ll_isstring(_fun, L, arg)) {
        color = ll_check_color_name(_fun, L, arg);
        if (color >= 1 << 24) {
            /* not a color name */
            const char *str = ll_check_string(_fun, L, arg);
            if (*str == '#')
                str++;
            /* expect hexadecimal digits for 0xRRGGBB */
            color = static_cast<l_int32>(strtol(str, nullptr, 16));
        }
        if (color >= 1 << 24) {
            /* >= 24 bits: 0xRRGGBBAA */
            r = (color >> 24) & 0xff;
            g = (color >> 16) & 0xff;
            b = (color >>  8) & 0xff;
            a = (color >>  0) & 0xff;
        } else {
            /* < 24 bits: 0x00RRGGBB */
            r = (color >> 16) & 0xff;
            g = (color >>  8) & 0xff;
            b = (color >>  0) & 0xff;
            a = 0xff;
        }
    }

    if (pr)
        *pr = r;
    if (pg)
        *pg = g;
    if (pb)
        *pb = b;
    if (pa)
        *pa = a;

    return 0;
}

/**
 * \brief ll_check_color
 * \brief Let's try our best to convert argument(s) to RGBA values
 * \param _fun calling function's name
 * \param L Lua state
 * \param arg index of the first parameter
 * \param ppixel pointer to RGBA value
 * \return 0 on success, or 1 on error
 */
int
ll_check_color(const char *_fun, lua_State *L, int arg, l_uint32 *ppixel)
{
    l_int32 r = 0;
    l_int32 g = 0;
    l_int32 b = 0;
    l_int32 a = 0;

    if (ppixel)
        *ppixel = 0;
    if (ll_check_color(_fun, L, arg, &r, &g, &b, &a))
        return 1;
    return composeRGBAPixel(r, g, b, a, ppixel);
}
