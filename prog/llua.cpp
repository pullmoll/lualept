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

#include <string.h>
#include <allheaders.h>
#include "../src/lualept.h"

#if defined(_MSC_VER)
/* Something goes wrong with importing this */
int LeptMsgSeverity = 0;
#endif

/**
 * @brief Return a printable character for byte ch
 * @param ch unsigned character
 * @return printable (ASCII) character
 */
char printable(l_uint8 ch)
{
    return ch < 32 || ch > 126 ? '.' : static_cast<char>(ch);
}

/**
 * @brief Quick and dirty hexdump of a byte array
 * @param bytes pointer to ll_bytes_t
 */
void hexdump(ll_bytes_t *bytes)
{
    size_t i, j;
    for (i = 0; i < bytes->size; i++) {
        if (0 == i % 16)
            printf("%04lx: ", i);
        printf(" %02x", bytes->data[i]);
        if (15 == (i % 16)) {
            printf(" [");
            for (j = i - 15; j <= i; j++)
                printf("%c", printable(bytes->data[j]));
            printf("]\n");
        }
    }
    for (j = i % 16; j < 16; j++)
        printf("   ");
    printf(" [");
    for (j = i - (i % 16); j < i; j++)
        printf("%c", printable(bytes->data[j]));
    for (j = i % 16; j < 16; j++)
        printf(" ");
    printf("]\n");
    printf("\n");
}

int main(int argc, char **argv)
{
    char buff[256];
    const char* progname = nullptr;
    const char* filename = nullptr;

    static Pix *i_pix = nullptr;
    static Box *i_box = nullptr;
    static Sarray *sa = nullptr;

    static Pix *o_pix = nullptr;
    static Box *o_box = nullptr;
    static bool ok = false;
    static ll_bytes_t bytes;

    static ll_global_var_t set_vars[] = {
        LL_GLOBAL(ll_box, "box", &i_box),
        LL_GLOBAL(ll_sarray, "sa", &sa),
        LL_GLOBAL(ll_pix, "pix", &i_pix),
        LL_SENTINEL
    };

    static ll_global_var_t get_vars[] = {
        LL_GLOBAL(ll_box, "box", &o_box),
        LL_GLOBAL(ll_boolean, "ok", &ok),
        LL_GLOBAL(ll_pix, "pix", &o_pix),
        LL_GLOBAL(ll_pbytes, "bytes", &bytes),
        LL_SENTINEL
    };
    l_int32 x, y, w, h, d;
    int res;

    progname = strrchr(argv[0], '\\');
    if (!progname)
        progname = strrchr(argv[0], '/');
    if (progname) {
        progname++;
    } else {
        progname = argv[0];
    }

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <script.lua>\n", progname);
        return 1;
    }
    filename = argv[1];

    /* Example for passing a Leptonica type to the script */

    /* Create the Sarray* sa */
    sa = sarrayCreate(8);
    snprintf(buff, sizeof(buff), "%s", "Hello, Lua!");
    sarrayAddString(sa, buff, L_COPY);

    /* Create the Pix* pix_in */
    i_pix = pixCreate(64, 64, 4);

    /* Create the Box* box_in */
    i_box = boxCreate(40, 40, 320, 240);

    res = ll_run(filename, nullptr, set_vars, get_vars);
    printf("%s: %s returned %d\n", progname, "ll_RunScript(script)", res);

    printf("%s: Pix* i_pix = %p\n", progname, reinterpret_cast<void *>(i_pix));
    printf("%s: Pix* o_pix = %p\n", progname, reinterpret_cast<void *>(o_pix));
    if (o_pix) {
        pixGetDimensions(o_pix, &w, &h, &d);
        printf("%s: w, h, d = %d, %d, %d\n", progname, w, h, d);
        pixDestroy(&o_pix);
    }
    printf("%s: Sarray* sa = %p\n", progname, reinterpret_cast<void *>(sa));

    printf("%s: Box* i_box = %p\n", progname, reinterpret_cast<void *>(i_box));
    printf("%s: Box* o_box = %p\n", progname, reinterpret_cast<void *>(o_box));
    if (o_box) {
        boxGetGeometry(o_box, &x, &y, &w, &h);
        printf("%s: x, y, w, h = %d, %d, %d, %d\n", progname, x, y, w, h);
        boxDestroy(&o_box);
    }
    printf("%s: bool ok = %s\n", progname, ok ? "true" : "false");
    printf("%s: ll_bytes_t bs = { data = %p, size = %lu }\n", progname,
           reinterpret_cast<void *>(bytes.data), bytes.size);
    hexdump(&bytes);
    free(bytes.data);
    return res;
}
