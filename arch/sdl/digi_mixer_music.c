/*
 * This is an alternate backend for the music system.
 * It uses SDL_mixer to provide a more reliable playback,
 * and allow processing of multiple audio formats.
 *
 *  -- MD2211 (2006-04-24)
 */

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <string.h>
#include <stdlib.h>

#include "args.h"
#include "hmp.h"
#include "digi_mixer_music.h"
#include "tsf_music.h"
#include "u_mem.h"
#include "console.h"

#ifdef _WIN32
extern int digi_win32_play_midi_song( char * filename, int loop );
#endif
Mix_Music *current_music = NULL;
static unsigned char *current_music_hndlbuf = NULL;


/*
 *  Plays a music file from an absolute path or a relative path
 */

int mix_play_file(char *filename, int loop, void (*hook_finished_track)())
{
	SDL_RWops *rw = NULL;
	PHYSFS_file *filehandle = NULL;
	char full_path[PATH_MAX];
	char *fptr;
	unsigned int bufsize = 0;

	mix_free_music();	// stop and free what we're already playing, if anything

	fptr = strrchr(filename, '.');

	if (fptr == NULL)
		return 0;

	// It's a .hmp! Try TinySoundFont first
	if (!d_stricmp(fptr, ".hmp"))
	{
		hmp2mid(filename, &current_music_hndlbuf, &bufsize);
		if (current_music_hndlbuf && bufsize > 0)
		{
			if (tsf_music_play(current_music_hndlbuf, bufsize, loop, hook_finished_track ? hook_finished_track : mix_free_music))
			{
				con_printf(CON_NORMAL, "Playing HMP song via TinySoundFont: %s\n", filename);
				return 1;
			}
		}
		rw = SDL_RWFromConstMem(current_music_hndlbuf,bufsize*sizeof(char));
		current_music = Mix_LoadMUS_RW(rw);
	}
	else if (!d_stricmp(fptr, ".mid"))
	{
		filehandle = PHYSFS_openRead(filename);
		if (filehandle != NULL)
		{
			current_music_hndlbuf = d_realloc(current_music_hndlbuf, sizeof(char *)*PHYSFS_fileLength(filehandle));
			bufsize = PHYSFS_read(filehandle, current_music_hndlbuf, sizeof(char), PHYSFS_fileLength(filehandle));
			PHYSFS_close(filehandle);
			if (current_music_hndlbuf && bufsize > 0)
			{
				if (tsf_music_play(current_music_hndlbuf, bufsize, loop, hook_finished_track ? hook_finished_track : mix_free_music))
				{
					con_printf(CON_NORMAL, "Playing MID song via TinySoundFont: %s\n", filename);
					return 1;
				}
			}
		}
	}

	// For non-MIDI formats (OGG, MP3, etc.), ensure TSF is stopped and hook detached
	tsf_music_stop();

	// try loading music via given filename
	if (!current_music)
		current_music = Mix_LoadMUS(filename);

	// allow the shell convention tilde character to mean the user's home folder
	// chiefly used for default jukebox level song music referenced in 'descent.m3u' for Mac OS X
	if (!current_music && *filename == '~')
	{
		snprintf(full_path, PATH_MAX, "%s%s", PHYSFS_getUserDir(),
				 &filename[1 + (!strncmp(&filename[1], PHYSFS_getDirSeparator(), strlen(PHYSFS_getDirSeparator())) ? 
				 strlen(PHYSFS_getDirSeparator()) : 0)]);
		current_music = Mix_LoadMUS(full_path);
		if (current_music)
			filename = full_path;	// used later for possible error reporting
	}
		

	// no luck. so it might be in Searchpath. So try to build absolute path
	if (!current_music)
	{
		PHYSFSX_getRealPath(filename, full_path);
		current_music = Mix_LoadMUS(full_path);
		if (current_music)
			filename = full_path;	// used later for possible error reporting
	}

	// still nothin'? Let's open via PhysFS in case it's located inside an archive
	if (!current_music)
	{
		filehandle = PHYSFS_openRead(filename);
		if (filehandle != NULL)
		{
			current_music_hndlbuf = d_realloc(current_music_hndlbuf, sizeof(char *)*PHYSFS_fileLength(filehandle));
			bufsize = PHYSFS_read(filehandle, current_music_hndlbuf, sizeof(char), PHYSFS_fileLength(filehandle));
			rw = SDL_RWFromConstMem(current_music_hndlbuf,bufsize*sizeof(char));
			PHYSFS_close(filehandle);
			current_music = Mix_LoadMUS_RW(rw);
		}
	}

	if (current_music)
	{
		Mix_PlayMusic(current_music, (loop ? -1 : 1));
		Mix_HookMusicFinished(hook_finished_track ? hook_finished_track : mix_free_music);
		return 1;
	}
	else
	{
		con_printf(CON_CRITICAL,"Music %s could not be loaded: %s\n", filename, Mix_GetError());
		mix_stop_music();
	}

	return 0;
}

// What to do when stopping song playback
void mix_free_music()
{
	tsf_music_free();
	Mix_HaltMusic();
	if (current_music)
	{
		Mix_FreeMusic(current_music);
		current_music = NULL;
	}
	if (current_music_hndlbuf)
	{
		d_free(current_music_hndlbuf);
		current_music_hndlbuf = NULL;
	}
}

void mix_set_music_volume(int vol)
{
	vol *= MIX_MAX_VOLUME/8;
	Mix_VolumeMusic(vol);
	tsf_music_set_volume(vol);
}

void mix_stop_music()
{
	tsf_music_stop();
	Mix_HaltMusic();
	if (current_music_hndlbuf)
	{
		d_free(current_music_hndlbuf);
		current_music_hndlbuf = NULL;
	}
}

void mix_pause_music()
{
	tsf_music_pause();
	Mix_PauseMusic();
}

void mix_resume_music()
{
	tsf_music_resume();
	Mix_ResumeMusic();
}

void mix_pause_resume_music()
{
	if (tsf_music_is_paused() || Mix_PausedMusic())
		mix_resume_music();
	else if (tsf_music_is_playing() || Mix_PlayingMusic())
		mix_pause_music();
}
