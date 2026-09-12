#ifndef _NX_SWITCH_H
#define _NX_SWITCH_H

#ifdef __SWITCH__
#include </opt/devkitpro/libnx/include/switch.h>
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int switch_get_text_input(const char *header, const char *initial_text, char *out_buffer, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif // _NX_SWITCH_H
