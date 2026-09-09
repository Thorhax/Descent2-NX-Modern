#ifndef _SDL_H_compat
#define _SDL_H_compat

#include <SDL2/SDL.h>
#include "SDL_keysym.h"
#include "SDL_audio.h"
#include "SDL_stdinc.h"

// SDL 1.2 video flags compatibility
#ifndef SDL_HWSURFACE
#define SDL_HWSURFACE       0x00000001
#endif
#ifndef SDL_ASYNCBLIT
#define SDL_ASYNCBLIT       0x00000004
#endif
#ifndef SDL_ANYFORMAT
#define SDL_ANYFORMAT       0x00000010
#endif
#ifndef SDL_HWPALETTE
#define SDL_HWPALETTE       0x00000020
#endif
#ifndef SDL_DOUBLEBUF
#define SDL_DOUBLEBUF       0x00000040
#endif
#ifndef SDL_FULLSCREEN
#define SDL_FULLSCREEN      0x00000080
#endif
#ifndef SDL_OPENGL
#define SDL_OPENGL          0x00000100
#endif
#ifndef SDL_RESIZABLE
#define SDL_RESIZABLE       0x00000200
#endif
#ifndef SDL_NOFRAME
#define SDL_NOFRAME         0x00000400
#endif

typedef enum {
    SDL_GRAB_QUERY = -1,
    SDL_GRAB_OFF = 0,
    SDL_GRAB_ON = 1
} SDL_GrabMode;

#define SDL_JoystickName(i) SDL_JoystickNameForIndex(i)

#ifdef __cplusplus
extern "C" {
#endif

int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags);
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
int SDL_Flip(SDL_Surface *screen);
int SDL_SetColors(SDL_Surface *surface, const SDL_Color *colors, int firstcolor, int ncolors);
int SDL_SetPalette(SDL_Surface *surface, int flags, const SDL_Color *colors, int firstcolor, int ncolors);
int SDL_WM_ToggleFullScreen(SDL_Surface *surface);
SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode);
void SDL_WM_SetCaption(const char *title, const char *icon);
SDL_Rect **SDL_ListModes(SDL_PixelFormat *format, Uint32 flags);
int SDL_EnableKeyRepeat(int delay, int interval);
int SDL_EnableUNICODE(int enable);
Uint8 *SDL_GetKeyState(int *numkeys);

#ifdef __cplusplus
}
#endif

#endif
