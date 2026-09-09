#include <SDL2/SDL.h>
#include "SDL/SDL.h"

static SDL_Window *s_window = NULL;
static SDL_Surface *s_screen_surface = NULL;

int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags)
{
    return 32;
}

SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
    if (!s_window) {
        s_window = SDL_CreateWindow(
            "D2X Switch",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280, 720,
            SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN
        );
        if (!s_window) {
            return NULL;
        }
    }
    s_screen_surface = SDL_GetWindowSurface(s_window);
    return s_screen_surface;
}

int SDL_Flip(SDL_Surface *screen)
{
    if (s_window) {
        return SDL_UpdateWindowSurface(s_window);
    }
    return 0;
}

int SDL_SetColors(SDL_Surface *surface, const SDL_Color *colors, int firstcolor, int ncolors)
{
    if (surface && surface->format && surface->format->palette) {
        return SDL_SetPaletteColors(surface->format->palette, colors, firstcolor, ncolors);
    }
    return 0;
}

int SDL_SetPalette(SDL_Surface *surface, int flags, const SDL_Color *colors, int firstcolor, int ncolors)
{
    return SDL_SetColors(surface, colors, firstcolor, ncolors);
}

int SDL_WM_ToggleFullScreen(SDL_Surface *surface)
{
    return 1;
}

SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode)
{
    if (s_window) {
        SDL_SetWindowGrab(s_window, (mode == SDL_GRAB_ON) ? SDL_TRUE : SDL_FALSE);
    }
    return mode;
}

void SDL_WM_SetCaption(const char *title, const char *icon)
{
    if (s_window && title) {
        SDL_SetWindowTitle(s_window, title);
    }
}

static SDL_Rect s_mode_1280_720 = {0, 0, 1280, 720};
static SDL_Rect s_mode_640_480  = {0, 0, 640, 480};
static SDL_Rect s_mode_320_200  = {0, 0, 320, 200};
static SDL_Rect *s_modes[] = {
    &s_mode_1280_720,
    &s_mode_640_480,
    &s_mode_320_200,
    NULL
};

SDL_Rect **SDL_ListModes(SDL_PixelFormat *format, Uint32 flags)
{
    return s_modes;
}

int SDL_EnableKeyRepeat(int delay, int interval)
{
    return 0;
}

int SDL_EnableUNICODE(int enable)
{
    return 0;
}

static Uint8 s_key_state[512] = {0};

Uint8 *SDL_GetKeyState(int *numkeys)
{
    if (numkeys) *numkeys = 512;
    return s_key_state;
}
