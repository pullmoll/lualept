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

#if defined(HAVE_SDL2)
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_video.h>

/**
 * @brief Set a pixel using %scale in the %image at %x, %y to color %r, %g, %b
 * @param image pointer to the SDL_Surface
 * @param x pixel column
 * @param y pixel row
 * @param r red value
 * @param g green value
 * @param b blue value
 * @param a alpha value
 */
static inline void
SetPixelRGB(SDL_Surface *image, int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = 1;
    rect.h = 1;
    uint32_t color = SDL_MapRGB(image->format, r, g, b);
    SDL_FillRect(image, &rect, color);
}

/**
 * @brief Set a pixel using %scale in the %image at %x, %y to color %r, %g, %b, %a
 * @param image pointer to the SDL_Surface
 * @param x pixel column
 * @param y pixel row
 * @param r red value
 * @param g green value
 * @param b blue value
 * @param a alpha value
 */
static inline void
SetPixelRGBA(SDL_Surface *image, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = 1;
    rect.h = 1;
    uint32_t color = SDL_MapRGBA(image->format, r, g, b, a);
    SDL_FillRect(image, &rect, color);
}

/**
 * @brief Set a pixel using %scale in the %image at %x, %y to %color
 * @param image pointer to the SDL_Surface
 * @param x pixel column
 * @param y pixel row
 * @param index pixel value
 */
static inline void
SetPixelIndex(SDL_Surface *image, int x, int y, uint32_t index)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = 1;
    rect.h = 1;
    SDL_FillRect(image, &rect, index);
}

/**
 * @brief Paint a Pix* %pix into the SDL_Surface %image using %scale
 * @param image pointer to the SDL_Surface
 * @param format pointer to the SDL_PixelFormat
 * @param pix pointer to the Pix
 */
static void
PaintImagePix(SDL_Surface *surface, Pix *pix)
{
    const int height = pixGetHeight(pix);
    const int depth = pixGetDepth(pix);
    const int wpl = pixGetWpl(pix);
    const int spp = pixGetSpp(pix);

    /* Copy the data from the Pix* to the image* */
    switch (depth) {
    case 1:
        /* 1 bit per pixel, binary or index */
        if (surface->format->palette) {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 32*wpl; x += 32) {
                    for (int z = 0; z < 32; z++) {
                        uint32_t index = GET_DATA_BIT(src, x + z);
                        SetPixelIndex(surface, x+z, y, index);
                    }
                }
            }
        } else {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 32*wpl; x += 32) {
                    for (int z = 0; z < 32; z++) {
                        uint8_t g = GET_DATA_BIT(src, x + z) ? 255 : 0;
                        SetPixelRGB(surface, x+z, y, g, g, g);
                    }
                }
            }
        }
        break;

    case 2:
        /* 2 bits per pixel, gray or index */
        if (surface->format->palette) {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 16*wpl; x += 16) {
                    for (int z = 0; z < 16; z++) {
                        uint32_t index = GET_DATA_DIBIT(src, x + z);
                        SetPixelIndex(surface, x+z, y, index);
                    }
                }
            }
        } else {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 16*wpl; x += 16) {
                    for (int z = 0; z < 16; z++) {
                        uint8_t g = 255 * GET_DATA_DIBIT(src, x + z) / 3;
                        SetPixelRGB(surface, x+z, y, g, g, g);
                    }
                }
            }
        }
        break;

    case 4:
        /* 4 bits per pixel, gray or cmap */
        if (surface->format->palette) {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 8*wpl; x += 8) {
                    for (int z = 0; z < 8; z++) {
                        uint32_t index = GET_DATA_QBIT(src, x + z);
                        SetPixelIndex(surface, x+z, y, index);
                    }
                }
            }
        } else {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 8*wpl; x += 8) {
                    for (int z = 0; z < 8; z++) {
                        uint8_t g = 255 * GET_DATA_QBIT(src, x + z) / 15;
                        SetPixelRGB(surface, x+z, y, g, g, g);
                    }
                }
            }
        }
        break;

    case 8:
        /* 8 bits per pixel, gray or cmap */
        if (surface->format->palette) {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 4*wpl; x += 4) {
                    for (int z = 0; z < 4; z++) {
                        uint32_t index = GET_DATA_BYTE(src, x+z);
                        SetPixelIndex(surface, x+z, y, index);
                    }
                }
            }
        } else {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 4*wpl; x += 4) {
                    for (int z = 0; z < 4; z++) {
                        uint8_t g = GET_DATA_BYTE(src, x+z);
                        SetPixelRGB(surface, x+z, y, g, g, g);
                    }
                }
            }
        }
        break;

    case 16:
        /* 16 bits per pixel gray */
        if (surface->format->palette) {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 2*wpl; x += 2) {
                    for (int b = 0; b < 2; b++) {
                        uint32_t index = GET_DATA_TWO_BYTES(src, x+b) / 256;
                        SetPixelIndex(surface, x+b, y, index / 256);
                    }
                }
            }
        } else {
            for (int y = 0; y < height; y++) {
                l_uint32* src = pixGetData(pix) + y * wpl;
                for (int x = 0; x < 2*wpl; x += 2) {
                    for (int b = 0; b < 2; b++) {
                        uint8_t g = GET_DATA_TWO_BYTES(src, x+b) / 256;
                        SetPixelRGB(surface, x+b, y, g, g, g);
                    }
                }
            }
        }
        break;

    case 24:
        /* 24 bits per pixel RGB */
        for (int y = 0; y < height; y++) {
            l_uint32* src = pixGetData(pix) + y * wpl;
            for (int x = 0; x < wpl; x++) {
                l_int32 r, g, b;
                l_uint32 pixel = GET_DATA_FOUR_BYTES(src, x);
                extractRGBValues(pixel, &r, &g, &b);
                SetPixelRGB(surface, x, y,
                              static_cast<uint8_t>(r),
                              static_cast<uint8_t>(g),
                              static_cast<uint8_t>(b)
                            );
            }
        }
        break;

    case 32:
        /* 32 bits per pixel RGBA with (spp = 4) or without (spp = 3) alpha */
        for (int y = 0; y < height; y++) {
            l_uint32* src = pixGetData(pix) + y * wpl;
            if (3 == spp) {
                for (int x = 0; x < wpl; x++) {
                    l_int32 r, g, b;
                    l_uint32 pixel = GET_DATA_FOUR_BYTES(src, x);
                    extractRGBValues(pixel, &r, &g, &b);
                    SetPixelRGB(surface, x, y,
                                  static_cast<uint8_t>(r),
                                  static_cast<uint8_t>(g),
                                  static_cast<uint8_t>(b)
                                );
                }
            } else {
                for (int x = 0; x < wpl; x++) {
                    l_int32 r, g, b, a;
                    l_uint32 pixel = GET_DATA_FOUR_BYTES(src, x);
                    extractRGBAValues(pixel, &r, &g, &b, &a);
                    SetPixelRGBA(surface, x, y,
                                  static_cast<uint8_t>(r),
                                  static_cast<uint8_t>(g),
                                  static_cast<uint8_t>(b),
                                  static_cast<uint8_t>(a)
                                 );
                }
            }
        }
        break;
    }
}

static float
ChangeScale(SDL_Window * window, float scale, int & swidth, int & sheight, const int width, const int height)
{
    FUNC("ChangeScale");
    swidth = static_cast<int>(width * scale);
    sheight = static_cast<int>(height * scale);
    SDL_SetWindowSize(window, swidth, sheight);
    return scale;
}

/**
 * @brief Fill a SDL_Palette with colors from a PixColormap*
 * @param palette pointer to the SDL_Palette
 * @param cmap pointer to the PixColormap
 * @param firstcolor first color index (usually 0)
 * @param ncolors number of colors (2, 16, 256)
 * @return pointer to allocated SDL_Palette
 */
static void
FillGrays(SDL_Palette* palette, int ncolors, int firstcolor = 0)
{
    FUNC("FillGrays");
    size_t size = static_cast<size_t>(ncolors) * sizeof(SDL_Color);
    SDL_Color *colors = reinterpret_cast<SDL_Color *>(malloc(size));
    if (!colors) {
        DBG(LOG_SDL2, "%s: failed to allocate %d colors\n", _fun,
            ncolors);
        return;
    }
    DBG(LOG_SDL2, "%s: filling %s*=%p with %d grays starting at %d\n", _fun,
        "SDL_Palette", reinterpret_cast<void *>(palette),
        ncolors,
        firstcolor);

    /* Gray ramp from 0 to 255 */
    for (int i = 0; i < ncolors; i++) {
        uint8_t g = static_cast<uint8_t>(255 *
                                         (i + firstcolor) /
                                         (ncolors + firstcolor - 1));
        colors[i].r = g;
        colors[i].g = g;
        colors[i].b = g;
        colors[i].a = 255;
    }

    if (SDL_SetPaletteColors(palette, colors, firstcolor, ncolors) < 0) {
        DBG(LOG_SDL2, "%s: SDL_SetPaletteColors() failed\n%s\n", _fun,
            SDL_GetError());
    }
    free(colors);
}

/**
 * @brief Fill a SDL_Palette with colors from a PixColormap*
 * @param palette pointer to the SDL_Palette
 * @param cmap pointer to the PixColormap
 * @param firstcolor first color index (usually 0)
 * @param ncolors number of colors (2, 16, 256)
 * @return pointer to allocated SDL_Palette
 */
static void
FillColors(SDL_Palette* palette, PixColormap *cmap, int ncolors, int firstcolor = 0)
{
    FUNC("FillColors");
    size_t size = static_cast<size_t>(ncolors) * sizeof(SDL_Color);
    SDL_Color *colors = reinterpret_cast<SDL_Color *>(malloc(size));

    if (!cmap) {
        DBG(LOG_SDL2, "%s: cmap is NULL\n", _fun);
        return;
    }
    if (!colors) {
        DBG(LOG_SDL2, "%s: failed to allocate %d colors\n", _fun,
            ncolors);
        return;
    }
    DBG(LOG_SDL2, "%s: filling %s*=%p with %d colors starting at %d from %s*=%p\n", _fun,
        "SDL_Palette", reinterpret_cast<void *>(palette),
        ncolors,
        firstcolor,
        LL_PIXCMAP, reinterpret_cast<void *>(cmap));

    for (int i = 0; i < ncolors; i++) {
        l_int32 r, g, b, a;
        l_uint32 color;
        pixcmapGetRGBA32(cmap, i, &color);
        extractRGBAValues(color, &r, &g, &b, &a);
        colors[i].r = static_cast<l_uint8>(r);
        colors[i].g = static_cast<l_uint8>(g);
        colors[i].b = static_cast<l_uint8>(b);
        colors[i].a = static_cast<l_uint8>(a);
    }

    if (SDL_SetPaletteColors(palette, colors, firstcolor, ncolors) < 0) {
        DBG(LOG_SDL2, "%s: SDL_SetPaletteColors() failed\n%s\n", _fun,
            SDL_GetError());
    }
    free(colors);
}

/**
 * \brief Display a Pix* using the SDL2 library
 *
 * FIXME: Use SDL_CreateSurfaceFrom() instead of drawing
 * the Pix* onto the surface.
 *
 * \param pix pointer to the Pix to display
 * \param x position on screen; <= 0 means undefinied
 * \param y position on screen; <= 0 means undefinied
 * \param scale initial scaling factor, or 0.0f to auto select
 * \return TRUE on success, or FALSE on error
 */
int
ViewSDL2(Pix* pix, const char* title, int x0, int y0, float dscale)
{
    FUNC("ShowSDL2");
    static l_float32 d_scales[8] = {1.0f, 0.75f, 0.66f, 0.50f, 0.33f, 0.25f, 0.10f, 0.05f};
    static l_float32 u_scales[8] = {1.0f, 1.25f, 1.50f, 1.75f, 2.00f, 2.50f, 2.75f, 3.00f};
    char info[256];
    const int width = pixGetWidth(pix);
    const int height = pixGetHeight(pix);
    const int depth = pixGetDepth(pix);
    const int wpl = pixGetWpl(pix);
    PixColormap *cmap = pixGetColormap(pix);
    const int xpos = x0 <= 0 ? SDL_WINDOWPOS_UNDEFINED : x0;
    const int ypos = y0 <= 0 ? SDL_WINDOWPOS_UNDEFINED : y0;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_PixelFormat *format = nullptr;
    SDL_Palette *palette = nullptr;
    SDL_Rect rect;
    bool quit = false;
    SDL_Event event;
    int swidth = 0;
    int sheight = 0;
    uint32_t rmask = 0u;
    uint32_t gmask = 0u;
    uint32_t bmask = 0u;
    uint32_t amask = 0u;
    int bits = 0;
    int result = TRUE;

    SDL_Init(SDL_INIT_VIDEO);

    if (SDL_GetDisplayBounds(0, &rect) < 0) {
        SDL_Quit();
        return FALSE;
    }

    /* use 75% of the display bounds */
    const float xscale = rect.w / width * 0.75f;
    const float yscale = rect.h / height * 0.75f;
    float scale = fabs(dscale) > 0.0001f ? dscale : L_MIN(xscale, yscale);

    /* scaled width and height */
    swidth = static_cast<int>(width * scale);
    sheight = static_cast<int>(height * scale);

    switch (depth) {
    case 1:
        bits = 8;
        format = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
        palette = format->palette ? format->palette : SDL_AllocPalette(2);
        if (cmap) {
            FillColors(palette, cmap, 2);
        } else {
            FillGrays(palette, 2);
        }
        break;

    case 2:
        bits = 8;
        format = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
        palette = format->palette ? format->palette : SDL_AllocPalette(16);
        if (cmap) {
            FillColors(palette, cmap, 4);
        } else {
            FillGrays(palette, 4);
        }
        break;

    case 4:
        bits = 8;
        format = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
        palette = SDL_AllocPalette(16);
        if (cmap) {
            FillColors(palette, cmap, 16);
        } else {
            FillGrays(palette, 16);
        }
        break;

    case 8:
        bits = 8;
        format = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
        palette = SDL_AllocPalette(256);
        if (cmap) {
            FillColors(palette, cmap, 256);
        } else {
            FillGrays(palette, 256);
        }
        break;

    case 16:
        bits = 8;
        format = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
        palette = format->palette ? format->palette : SDL_AllocPalette(256);
        if (cmap) {
            FillColors(palette, cmap, 256);
        } else {
            FillGrays(palette, 256);
        }
        break;

    case 24:
        bits = 24;
        format = SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);
        rmask = 0xfful << L_RED_SHIFT;
        gmask = 0xfful << L_GREEN_SHIFT;
        bmask = 0xfful << L_BLUE_SHIFT;
        amask = 0;
        break;

    case 32:
        bits = 32;
        format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
        rmask = 0xfful << L_RED_SHIFT;
        gmask = 0xfful << L_GREEN_SHIFT;
        bmask = 0xfful << L_BLUE_SHIFT;
        amask = 0xfful << L_ALPHA_SHIFT;
        break;
    }

    snprintf(info, sizeof(info), "Pix*: %s%dx%dx%d, %s=%d, %s=%d, %s=%.3g",
             cmap ? "cmap, " : "",
             width, height, depth,
             "bpl", 4 * wpl,
             "bits", bits,
             "scale", static_cast<double>(scale)
             );
    DBG(LOG_SDL2, "%s: %s\n", _fun, info);

    if (!format) {
        result = FALSE;
        goto failure;
    }

    if (palette) {
        SDL_SetPixelFormatPalette(format, palette);
    }
    surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format->format);
    if (!surface) {
        DBG(LOG_SDL2, "%s: could not create surface\n", _fun);
        result = FALSE;
        goto failure;
    }

    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
    SDL_SetSurfacePalette(surface, format->palette);
    PaintImagePix(surface, pix);

    window = SDL_CreateWindow(title ? title : info, xpos, ypos, swidth, sheight, SDL_WINDOW_RESIZABLE);
    if (!window) {
        DBG(LOG_SDL2, "%s: could not create window\n", _fun);
        result = FALSE;
        goto failure;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        DBG(LOG_SDL2, "%s: could not create renderer\n", _fun);
        result = FALSE;
        goto failure;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = nullptr;
    if (!texture) {
        DBG(LOG_SDL2, "%s: could not create texture\n", _fun);
        result = FALSE;
        goto failure;
    }

    /* Wait for the user to press Escape or close the window */
    while (!quit) {
        SDL_WaitEvent(&event);

        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                quit = true;
                break;
            case 'q':
                quit = true;
                break;
            case SDLK_MINUS:
            case SDLK_KP_MINUS:
                scale = ChangeScale(window, scale * 0.98f, swidth, sheight, width, height);
                break;
            case SDLK_PLUS:
            case SDLK_KP_PLUS:
                scale = ChangeScale(window, scale * 1.02f, swidth, sheight, width, height);
                break;
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                if (SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT)) {
                    /* Scale to 100%, 75%, 66%, 50%, 33% ... */
                    int n = event.key.keysym.sym - '0';
                    scale = ChangeScale(window, d_scales[n], swidth, sheight, width, height);
                } else {
                    /* Scale to 100%, 125%, 150%, 175%, 200% ... */
                    int n = event.key.keysym.sym - '0';
                    scale = ChangeScale(window, u_scales[n], swidth, sheight, width, height);
                }
                break;
            case 's':
                /* toggle scaling on / off */
                if (fabs(dscale - scale) < 0.001f) {
                    scale = ChangeScale(window, L_MIN(xscale, yscale), swidth, sheight, width, height);
                } else {
                    scale = ChangeScale(window, dscale, swidth, sheight, width, height);
                }
                break;
            }
            break;

        case SDL_QUIT:
            quit = true;
            break;
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

failure:
    SDL_FreeSurface(surface);
    SDL_FreeFormat(format);
    SDL_FreePalette(palette);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}
#else
int
ViewSDL2(Pix* pix, const char* title, int x0, int y0, float dscale)
{
    UNUSED(pix);
    UNUSED(title);
    UNUSED(x0);
    UNUSED(y0);
    UNUSED(dscale);
    return FALSE;
}
#endif
