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

int main(int argc, char **argv)
{
    char buff[256];
    const char* progname = nullptr;
    const char* script = nullptr;

    static Box *box_in = nullptr;
    static Sarray *sa = nullptr;
    static Box *box_out = nullptr;
    static bool ok = false;

    static ll_global_var_t set_vars[] = {
        {LL_SARRAY,     "sa",   &sa},
        {LL_BOX,        "box",  &box_in},
        LL_SENTINEL
    };

    static ll_global_var_t get_vars[] = {
        {LL_BOX,        "box",  &box_out},
        {LL_BOOLEAN,    "ok",   &ok},
        LL_SENTINEL
    };
    l_int32 x, y, w, h;
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
    script = argv[1];

    /* Example for passing a Leptonica type to the script */

    /* Create the Sarray* sa */
    sa = sarrayCreate(8);
    snprintf(buff, sizeof(buff), "%s", "Hello, Lua!");
    sarrayAddString(sa, buff, L_COPY);
    /* Create the Box* box_in */
    box_in = boxCreate(40, 40, 320, 240);

    /* Register the globals which go in */
    ll_set_globals(set_vars);
    /* Register the globals which come out */
    ll_get_globals(get_vars);

    res = ll_RunScript(script);
    printf("%s: %s returned %d\n", progname, "ll_RunScript(script)", res);

    printf("%s: Box* box_out = %p\n", progname, reinterpret_cast<void *>(box_out));
    if (box_out) {
        boxGetGeometry(box_out, &x, &y, &w, &h);
        printf("%s: x, y, w, h = %d, %d, %d, %d\n", progname, x, y, w, h);
    }
    printf("%s: bool ok = %s\n", progname, ok ? "true" : "false");
    return res;
}
