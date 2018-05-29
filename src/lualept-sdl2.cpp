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

static void
SetPixel(SDL_Surface *image, float scale, int x, int y, uint32_t color)
{
    SDL_Rect rect;
    rect.x = static_cast<int>(x * scale);
    rect.y = static_cast<int>(y * scale);
    rect.w = static_cast<int>(scale + 0.5f);
    rect.h = static_cast<int>(scale + 0.5f);
    SDL_FillRect(image, &rect, color);
}

static l_uint32
MakeColor(SDL_Surface *image, l_int32 red, l_int32 green, l_int32 blue, l_int32 alpha)
{
    const l_uint32 color = SDL_MapRGBA(image->format,
                                       static_cast<l_uint8>(red),
                                       static_cast<l_uint8>(green),
                                       static_cast<l_uint8>(blue),
                                       static_cast<l_uint8>(alpha));
    return color;
}

static void
PaintImagePix(SDL_Surface *image, Pix *pix, float scale)
{
    l_uint32 *srcdata = pixGetData(pix);
    PixColormap *cmap = pixGetColormap(pix);
    const int height = pixGetHeight(pix);
    const int depth = pixGetDepth(pix);
    const int wpl = pixGetWpl(pix);
    const int spp = pixGetSpp(pix);

    /* Copy the data from the Pix* to the image* */
    switch (depth) {
    case 1:
        for (int y = 0; y < height; y++) {
            l_uint32* src = srcdata + y * wpl;
            for (int x = 0; x < 32*wpl; x += 32) {
                if (cmap) {
                    for (int b = 0; b < 32; b++) {
                        l_int32 red, green, blue;
                        l_uint32 pixel, color;
                        pixcmapGetColor32(cmap, GET_DATA_BIT(src, x + b), &pixel);
                        extractRGBValues(pixel, &red, &green, &blue);
                        color = MakeColor(image, red, green, blue, 255);
                        SetPixel(image, scale, x+b, y, color);
                    }
                } else {
                    for (int b = 0; b < 32; b++) {
                        l_int32 gray = 255 * GET_DATA_BIT(src, x + b);
                        l_uint32 color = MakeColor(image, gray, gray, gray, 255);
                        SetPixel(image, scale, x+b, y, color);
                    }
                }
            }
        }
        break;

    case 2:
        for (int y = 0; y < height; y++) {
            l_uint32* src = srcdata + y * wpl;
            for (int x = 0; x < 16*wpl; x += 16) {
                if (cmap) {
                    for (int b = 0; b < 16; b++) {
                        l_int32 red, green, blue;
                        l_uint32 pixel, color;
                        pixcmapGetColor32(cmap, GET_DATA_DIBIT(src, x + b), &pixel);
                        extractRGBValues(pixel, &red, &green, &blue);
                        color = MakeColor(image, red, green, blue, 255);
                        SetPixel(image, scale, x+b, y, color);
                    }
                } else {
                    for (int b = 0; b < 16; b++) {
                        l_int32 gray = 255 * GET_DATA_DIBIT(src, x + b) / 3;
                        l_uint32 color = MakeColor(image, gray, gray, gray, 255);
                        SetPixel(image, scale, x+b, y, color);
                    }
                }
            }
        }
        break;

    case 4:
        for (int y = 0; y < height; y++) {
            l_uint32* src = srcdata + y * wpl;
            for (int x = 0; x < 8*wpl; x += 8) {
                if (cmap) {
                    for (int b = 0; b < 8; b++) {
                        l_int32 red, green, blue;
                        l_uint32 pixel, color;
                        pixcmapGetColor32(cmap, GET_DATA_QBIT(src, x + b), &pixel);
                        extractRGBValues(pixel, &red, &green, &blue);
                        color = MakeColor(image, red, green, blue, 255);
                        SetPixel(image, scale, x+b, y, color);
                    }
                } else {
                    for (int b = 0; b < 8; b++) {
                        l_int32 gray = 255 * GET_DATA_QBIT(src, x + b) / 15;
                        const l_uint32 color = MakeColor(image, gray, gray, gray, 255);
                        SetPixel(image, scale, x+b, y, color);
                    }
                }
            }
        }
        break;

    case 8:
        for (int y = 0; y < height; y++) {
            l_uint32* src = srcdata + y * wpl;
            for (int x = 0; x < 4*wpl; x += 4) {
                if (cmap) {
                    for (int b = 0; b < 4; b++) {
                        l_int32 red, green, blue;
                        l_uint32 pixel, color;
                        pixcmapGetColor32(cmap, GET_DATA_BYTE(src, x+b), &pixel);
                        extractRGBValues(pixel, &red, &green, &blue);
                        color = MakeColor(image, red, green, blue, 255);
                        SetPixel(image, scale, x+b, y, color);
                    }
                } else {
                    for (int b = 0; b < 4; b++) {
                        l_int32 gray = GET_DATA_BYTE(src, x+b);
                        l_uint32 color = MakeColor(image, gray, gray, gray, 255);
                        SetPixel(image, scale, x+b, y, color);
                    }
                }
            }
        }
        break;

    case 16:
        for (int y = 0; y < height; y++) {
            l_uint32* src = srcdata + y * wpl;
            for (int x = 0; x < 2*wpl; x += 2) {
                for (int b = 0; b < 2; b++) {
                    l_int32 gray = GET_DATA_TWO_BYTES(src, x+b) / 256;
                    const l_uint32 color = MakeColor(image, gray, gray, gray, 255);
                    SetPixel(image, scale, x+b, y, color);
                }
            }
        }
        break;

    case 24:
        for (int y = 0; y < height; y++) {
            l_uint32* src = srcdata + y * wpl;
            for (int x = 0; x < wpl; x++) {
                l_int32 red, green, blue;
                l_uint32 pixel = GET_DATA_FOUR_BYTES(src, x), color;
                extractRGBValues(pixel, &red, &green, &blue);
                color = MakeColor(image, red, green, blue, 255);
                SetPixel(image, scale, x, y, color);
            }
        }
        break;

    case 32:
        for (int y = 0; y < height; y++) {
            l_uint32* src = srcdata + y * wpl;
            if (3 == spp) {
                for (int x = 0; x < wpl; x++) {
                    l_int32 red, green, blue;
                    l_uint32 pixel = GET_DATA_FOUR_BYTES(src, x), color;
                    extractRGBValues(pixel, &red, &green, &blue);
                    color = MakeColor(image, red, green, blue, 255);
                    SetPixel(image, scale, x, y, color);
                }
            } else {
                for (int x = 0; x < wpl; x++) {
                    l_int32 red, green, blue, alpha;
                    l_uint32 pixel = GET_DATA_FOUR_BYTES(src, x), color;
                    extractRGBAValues(pixel, &red, &green, &blue, &alpha);
                    color = MakeColor(image, red, green, blue, alpha);
                    SetPixel(image, scale, x, y, color);
                }
            }
        }
        break;
    }
}
/**
 * @brief Display a Pix* using the SDL2 library
 * @param pix pointer to the Pix to display
 * @param x position on screen; <= 0 means undefinied
 * @param y position on screen; <= 0 means undefinied
 * @return
 */
int
DisplaySDL2(Pix* pix, int x0, int y0, const char* title)
{
    FUNC("DisplaySDL2");

    const int width = pixGetWidth(pix);
    const int height = pixGetHeight(pix);
    const int xpos = x0 <= 0 ? SDL_WINDOWPOS_UNDEFINED : x0;
    const int ypos = y0 <= 0 ? SDL_WINDOWPOS_UNDEFINED : y0;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Rect rect;

    SDL_Init(SDL_INIT_VIDEO);

    if (SDL_GetDisplayBounds(0, &rect) < 0) {
        SDL_Quit();
        return FALSE;
    }

    const float xscale = rect.w / width * 0.75f;
    const float yscale = rect.h / height * 0.75f;
    float scale = L_MIN(xscale, yscale);
    int swidth = static_cast<int>(width * scale);
    int sheight = static_cast<int>(height * scale);

    window = SDL_CreateWindow(title, xpos, ypos, swidth, sheight, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, 0);

    const uint32_t rmask = 0xfful << L_RED_SHIFT;
    const uint32_t gmask = 0xfful << L_GREEN_SHIFT;
    const uint32_t bmask = 0xfful << L_BLUE_SHIFT;
    const uint32_t amask = 0xfful << L_ALPHA_SHIFT;
    SDL_Surface *image = SDL_CreateRGBSurface(0, swidth, sheight, 32, rmask, gmask, bmask, amask);

    if (!image) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return FALSE;
    }

    PaintImagePix(image, pix, scale);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    if (!texture) {
        SDL_FreeSurface(image);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return FALSE;
    }

    bool quit = false;
    SDL_Event event;

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
            case 's':
                /* toggle scaling on / off */
                if (1.0f == scale) {
                    scale = L_MIN(xscale, yscale);
                } else {
                    scale = 1.0f;
                }
                swidth = static_cast<int>(width * scale);
                sheight = static_cast<int>(height * scale);
                SDL_SetWindowSize(window, swidth, sheight);
                break;
            }
            break;
        case SDL_QUIT:
            quit = true;
            break;
        }
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return TRUE;
}
#else
int
DisplaySDL2(Pix* pix, int x, int y, const char* title)
{
    UNUSED(pix);
    UNUSED(x);
    UNUSED(y);
    UNUSED(title);
    return FALSE;
}
#endif