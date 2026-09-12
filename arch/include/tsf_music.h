#ifndef _TSF_MUSIC_H
#define _TSF_MUSIC_H

#ifdef __cplusplus
extern "C" {
#endif

int  tsf_music_init(void);
void tsf_music_close(void);
int  tsf_music_play(const unsigned char *midibuf, unsigned int midilen, int loop, void (*hook_finished)());
void tsf_music_stop(void);
void tsf_music_free(void);
void tsf_music_pause(void);
void tsf_music_resume(void);
int  tsf_music_is_playing(void);
int  tsf_music_is_paused(void);
void tsf_music_set_volume(int vol);

#ifdef __cplusplus
}
#endif

#endif // _TSF_MUSIC_H
