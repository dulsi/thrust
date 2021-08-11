
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "thrust_t.h"
#include "sound.h"

ui8 sound_boom[]   = { 0 };
ui8 sound_boom2[]  = { 0 };
ui8 sound_harp[]   = { 0 };
ui8 sound_engine[] = { 0 };
ui8 sound_blip[]   = { 0 };
unsigned int sound_boom_len   = 1;
unsigned int sound_boom2_len  = 1;
unsigned int sound_harp_len   = 1;
unsigned int sound_engine_len = 1;
unsigned int sound_blip_len   = 1;

int
sound_depends_on_graphics()
{
  return 0;
}

int
sound_init(const sound_t *sounds)
{
  return -1;
}

int
sound_exit(void)
{
  return 0;
}

int
sound_play(int sound, int channel)
{
  return 0;
}

int
sound_stop(int channel)
{
  return 0;
}

const char *
sound_name(void)
{
  static char name[] = "Silence";

  return name;
}
