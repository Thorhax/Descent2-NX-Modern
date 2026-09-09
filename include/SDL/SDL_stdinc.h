#ifndef _SDL_stdinc_h_compat
#define _SDL_stdinc_h_compat

#include <SDL2/SDL_stdinc.h>

#ifndef SDL_putenv
static inline int SDL_putenv(const char *variable) {
    // SDL_putenv expected "VAR=VAL" format in SDL 1.2
    char buf[256];
    char *eq;
    if (!variable) return -1;
    strncpy(buf, variable, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    eq = strchr(buf, '=');
    if (eq) {
        *eq = '\0';
        return SDL_setenv(buf, eq + 1, 1);
    }
    return 0;
}
#endif

#endif
