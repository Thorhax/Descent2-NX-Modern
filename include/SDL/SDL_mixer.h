#ifndef _SDL_mixer_h_compat
#define _SDL_mixer_h_compat

#include <SDL2/SDL_mixer.h>

#define Mix_LoadMUS_RW(rw) Mix_LoadMUS_RW(rw, 0)

#endif
