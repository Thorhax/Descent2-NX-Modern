#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#define TSF_IMPLEMENTATION
#include "tsf.h"

#define TML_IMPLEMENTATION
#include "tml.h"

#include "tsf_music.h"
#include "console.h"

static tsf *g_tsf = NULL;
static tml_message *g_midi = NULL;
static tml_message *g_midi_current = NULL;
static double g_midi_time_ms = 0.0;
static int g_midi_loop = 0;
static int g_midi_playing = 0;
static int g_midi_paused = 0;
static float g_music_volume = 1.0f;
static void (*g_hook_finished)() = NULL;
static int g_tsf_initialized = 0;

static void tsf_music_callback(void *udata, Uint8 *stream, int len)
{
	(void)udata;
	if (!g_tsf || !g_midi || !g_midi_playing || g_midi_paused)
	{
		memset(stream, 0, len);
		return;
	}

	int samples_needed = len / (2 * sizeof(short)); // stereo 16-bit
	short *buf = (short *)stream;

	while (samples_needed > 0 && g_midi_playing)
	{
		int block = samples_needed > 64 ? 64 : samples_needed;
		double block_ms = (block * 1000.0) / 48000.0;
		double next_time = g_midi_time_ms + block_ms;

		while (g_midi_current && g_midi_current->time <= next_time)
		{
			switch (g_midi_current->type)
			{
				case TML_NOTE_ON:
					if (g_midi_current->velocity > 0)
						tsf_channel_note_on(g_tsf, g_midi_current->channel, g_midi_current->key, g_midi_current->velocity / 127.0f);
					else
						tsf_channel_note_off(g_tsf, g_midi_current->channel, g_midi_current->key);
					break;
				case TML_NOTE_OFF:
					tsf_channel_note_off(g_tsf, g_midi_current->channel, g_midi_current->key);
					break;
				case TML_CONTROL_CHANGE:
					tsf_channel_midi_control(g_tsf, g_midi_current->channel, g_midi_current->control, g_midi_current->control_value);
					break;
				case TML_PROGRAM_CHANGE:
					tsf_channel_set_presetnumber(g_tsf, g_midi_current->channel, g_midi_current->program, (g_midi_current->channel == 9));
					break;
				case TML_PITCH_BEND:
					tsf_channel_set_pitchwheel(g_tsf, g_midi_current->channel, g_midi_current->pitch_bend);
					break;
				default:
					break;
			}
			g_midi_current = g_midi_current->next;
		}

		tsf_render_short(g_tsf, buf, block, 0);

		buf += block * 2;
		samples_needed -= block;
		g_midi_time_ms = next_time;

		if (!g_midi_current)
		{
			if (g_midi_loop)
			{
				g_midi_current = g_midi;
				g_midi_time_ms = 0.0;
			}
			else
			{
				g_midi_playing = 0;
				if (samples_needed > 0)
					memset(buf, 0, samples_needed * 2 * sizeof(short));
				if (g_hook_finished)
					g_hook_finished();
				break;
			}
		}
	}
}

int tsf_music_init(void)
{
	if (g_tsf_initialized)
		return (g_tsf != NULL);

	g_tsf_initialized = 1;

	const char *sf_paths[] = {
		"sdmc:/switch/descent1/soundfont.sf2",
		"sdmc:/switch/descent2/soundfont.sf2",
		"soundfont.sf2",
		"romfs:/soundfont.sf2",
		"romfs:/gzdoom.sf2",
		"/switch/descent1/soundfont.sf2",
		"/switch/descent2/soundfont.sf2",
		NULL
	};

	const char *chosen_path = NULL;
	for (int i = 0; sf_paths[i] != NULL; i++)
	{
		FILE *fp = fopen(sf_paths[i], "rb");
		if (fp)
		{
			fclose(fp);
			chosen_path = sf_paths[i];
			break;
		}
	}

	if (!chosen_path)
	{
		con_printf(CON_URGENT, "TSF: No soundfont.sf2 found!\n");
		return 0;
	}

	con_printf(CON_NORMAL, "TSF: Loading soundfont from %s\n", chosen_path);
	g_tsf = tsf_load_filename(chosen_path);
	if (!g_tsf)
	{
		con_printf(CON_URGENT, "TSF: Failed to load soundfont: %s\n", chosen_path);
		return 0;
	}

	tsf_set_output(g_tsf, TSF_STEREO_INTERLEAVED, 48000, 0.0f);
	tsf_set_volume(g_tsf, g_music_volume);
	con_printf(CON_NORMAL, "TSF: SoundFont initialized successfully (%d presets)\n", tsf_get_presetcount(g_tsf));
	return 1;
}

void tsf_music_close(void)
{
	tsf_music_stop();
	Mix_HookMusic(NULL, NULL);
	if (g_midi)
	{
		tml_free(g_midi);
		g_midi = NULL;
		g_midi_current = NULL;
	}
	if (g_tsf)
	{
		tsf_close(g_tsf);
		g_tsf = NULL;
	}
	g_tsf_initialized = 0;
}

int tsf_music_play(const unsigned char *midibuf, unsigned int midilen, int loop, void (*hook_finished)())
{
	if (!g_tsf && !tsf_music_init())
		return 0;

	if (!midibuf || midilen == 0)
		return 0;

	SDL_LockAudio();

	if (g_midi)
	{
		tml_free(g_midi);
		g_midi = NULL;
		g_midi_current = NULL;
	}

	g_midi = tml_load_memory(midibuf, (int)midilen);
	if (!g_midi)
	{
		SDL_UnlockAudio();
		con_printf(CON_URGENT, "TSF: Failed to parse MIDI data!\n");
		return 0;
	}

	g_midi_current = g_midi;
	g_midi_time_ms = 0.0;
	g_midi_loop = loop;
	g_hook_finished = hook_finished;
	g_midi_playing = 1;
	g_midi_paused = 0;

	tsf_reset(g_tsf);
	tsf_set_volume(g_tsf, g_music_volume);

	Mix_HookMusic(tsf_music_callback, NULL);

	SDL_UnlockAudio();
	return 1;
}

void tsf_music_stop(void)
{
	SDL_LockAudio();
	g_midi_playing = 0;
	g_midi_paused = 0;
	if (g_tsf)
		tsf_reset(g_tsf);
	Mix_HookMusic(NULL, NULL);
	SDL_UnlockAudio();
}

void tsf_music_free(void)
{
	tsf_music_stop();
	SDL_LockAudio();
	if (g_midi)
	{
		tml_free(g_midi);
		g_midi = NULL;
		g_midi_current = NULL;
	}
	SDL_UnlockAudio();
}

void tsf_music_pause(void)
{
	g_midi_paused = 1;
}

void tsf_music_resume(void)
{
	g_midi_paused = 0;
}

int tsf_music_is_playing(void)
{
	return (g_midi_playing && !g_midi_paused);
}

int tsf_music_is_paused(void)
{
	return g_midi_paused;
}

void tsf_music_set_volume(int vol)
{
	if (vol < 0) vol = 0;
	if (vol > 128) vol = 128;
	g_music_volume = (float)vol / 128.0f;
	if (g_tsf)
		tsf_set_volume(g_tsf, g_music_volume);
}
