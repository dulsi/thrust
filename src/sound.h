
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef SOUND_H
#define SOUND_H

#define SOUND_SAMPLES 5
#define SOUND_CHANNELS 4
#if defined(HAVE_LINUX_SOUND) || defined(HAVE_CYGWIN_SOUND)
#define SAMPLE_RATE 11000
#elif defined(HAVE_SUN_SOUND) || defined(HAVE_HP_SOUND)
#define SAMPLE_RATE 11025
#elif defined(HAVE_DIRECTSOUND)
#define SAMPLE_RATE 11025
#endif

typedef struct {
  ui8 *data;
  int len;
  int loop;
} sound_t;

#ifdef __STDC__
int sound_depends_on_graphics(void);
int sound_init(const sound_t *sounds);
int sound_exit(void);
int sound_play(int sound, int channel);
int sound_stop(int channel);
const char *sound_name(void);
#endif

#endif /* SOUND_H */
