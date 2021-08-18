
/* Written by Dennis Payne, dulsi@identicalsoftware.com */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "thrust_t.h"
#include "sound.h"
#include <SDL_mixer.h>

Mix_Chunk sdlSounds[SOUND_SAMPLES];
int sdlLoop[SOUND_SAMPLES];

int
sound_depends_on_graphics()
{
  return 1;
}

int
sound_init(const sound_t *sounds)
{
  for (int i = 0; i < SOUND_SAMPLES; i++)
  {
    sdlSounds[i].allocated = 0;
    sdlSounds[i].abuf = sounds[i].data;
    sdlSounds[i].alen = sounds[i].len;
    sdlSounds[i].volume = 128;
    sdlLoop[i] = (sounds[i].loop == 1 ? -1 : 0);
  }
  return Mix_OpenAudio(SAMPLE_RATE, AUDIO_U8, 2, 1024);
}

int
sound_exit(void)
{
  Mix_CloseAudio();
  return 0;
}

int
sound_play(int sound, int channel)
{
  Mix_PlayChannel(channel, &sdlSounds[sound], sdlLoop[sound]);
  return 0;
}

int
sound_stop(int channel)
{
  Mix_HaltChannel(channel);
  return 0;
}

const char *
sound_name(void)
{
  static char name[] = "SDLMixer";

  return name;
}
