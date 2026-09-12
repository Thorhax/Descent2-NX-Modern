// Holds the main init and de-init functions for arch-related program parts

#include <SDL/SDL.h>
#ifdef __SWITCH__
#include </opt/devkitpro/libnx/include/switch.h>
#endif
#include "songs.h"
#include "key.h"
#include "digi.h"
#include "mouse.h"
#include "joy.h"
#include "gr.h"
#include "dxxerror.h"
#include "text.h"
#include "args.h"
#include "config.h"

void arch_close(void)
{
	songs_uninit();

	gr_close();

	if (!GameArg.CtlNoJoystick)
		joy_close();

	mouse_close();


	if (!GameArg.SndNoSound)
	{
		digi_close();
	}

	key_close();

	SDL_Quit();

#ifdef __SWITCH__
	romfsExit();
#endif
}

void arch_init(void)
{
	int t;

#ifdef __SWITCH__
	romfsInit();
#endif

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		Error("SDL library initialisation failed: %s.",SDL_GetError());

	key_init();

	digi_select_system( GameArg.SndDisableSdlMixer ? SDLAUDIO_SYSTEM : SDLMIXER_SYSTEM );

	if (!GameArg.SndNoSound)
		digi_init();

	mouse_init();

	if (!GameArg.CtlNoJoystick)
		joy_init();

	if ((t = gr_init(0)) != 0)
		Error(TXT_CANT_INIT_GFX,t);

	atexit(arch_close);
}

int switch_get_text_input(const char *header, const char *initial_text, char *out_buffer, size_t max_len)
{
#ifdef __SWITCH__
	SwkbdConfig kbd;
	Result rc;

	if (!out_buffer || max_len <= 1)
		return 0;

	rc = swkbdCreate(&kbd, 0);
	if (R_FAILED(rc))
		return 0;

	swkbdConfigMakePresetDefault(&kbd);
	if (header && header[0])
		swkbdConfigSetHeaderText(&kbd, header);
	if (initial_text && initial_text[0])
		swkbdConfigSetInitialText(&kbd, initial_text);
	swkbdConfigSetStringLenMax(&kbd, (u32)(max_len - 1));

	rc = swkbdShow(&kbd, out_buffer, max_len);
	swkbdClose(&kbd);

	return R_SUCCEEDED(rc);
#else
	(void)header; (void)initial_text; (void)out_buffer; (void)max_len;
	return 0;
#endif
}

