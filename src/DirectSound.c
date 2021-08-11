
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <windows.h>
#define DIRECTSOUND_VERSION 0x0700
#define _LPCWAVEFORMATEX_DEFINED
#include <dsound.h>
#include "thrust_t.h"
#include "sound.h"

extern HANDLE hWnd;

static LPDIRECTSOUND pDS;
static LPDIRECTSOUNDBUFFER *pDSB;
static LPDIRECTSOUNDBUFFER *pDSBChannel;
static const sound_t *sound_sounds = NULL;

int
sound_depends_on_graphics()
{
  return 1;
}

int
sound_init(const sound_t *sounds)
{
  HRESULT hr;
  DSBUFFERDESC DSBufferDesc;
  WAVEFORMATEX WaveFormatEx;
  int i;
  ui8 *pBuffer;
  DWORD dwSize;

  sound_sounds = sounds;

  if(sounds == NULL)
    return -1;
    
  hr = DirectSoundCreate(NULL, &pDS, NULL);
  if(hr != DS_OK)
    return -1;

  pDSB = malloc(SOUND_SAMPLES * sizeof(LPDIRECTSOUNDBUFFER));
  if(pDSB == NULL)
    goto cleanup;

  for(i = 0; i < SOUND_SAMPLES; ++i)
    pDSB[i] = NULL;

  pDSBChannel = malloc(SOUND_CHANNELS * sizeof(LPDIRECTSOUNDBUFFER));
  if(pDSBChannel == NULL)
    goto cleanup;

  for(i = 0; i < SOUND_CHANNELS; ++i)
    pDSBChannel[i] = NULL;

  hr = IDirectSound_SetCooperativeLevel(pDS, hWnd, DSSCL_PRIORITY);
  if(hr != DS_OK)
    goto cleanup;

  WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
  WaveFormatEx.nChannels = 1;
  WaveFormatEx.nSamplesPerSec = SAMPLE_RATE;
  WaveFormatEx.nAvgBytesPerSec = SAMPLE_RATE;
  WaveFormatEx.nBlockAlign = 1;
  WaveFormatEx.wBitsPerSample = 8;
  WaveFormatEx.cbSize = 0;
  for(i = 0; i < SOUND_SAMPLES; ++i) {
    DSBufferDesc.dwSize = sizeof(DSBufferDesc);
    DSBufferDesc.dwFlags = DSBCAPS_STATIC;
    DSBufferDesc.dwBufferBytes = sounds[i].len;
    DSBufferDesc.dwReserved = 0;
    DSBufferDesc.lpwfxFormat = &WaveFormatEx;

    hr = IDirectSound_CreateSoundBuffer(pDS, &DSBufferDesc, &pDSB[i], NULL);
    if(hr != DS_OK)
      goto cleanup;
    
    hr = IDirectSoundBuffer_Lock(pDSB[i], 0, 0, (void**)&pBuffer, &dwSize, 0, 0, DSBLOCK_ENTIREBUFFER);
    if(hr != DS_OK)
      goto cleanup;

    if(dwSize < sounds[i].len) {
      IDirectSoundBuffer_Unlock(pDSB[i], pBuffer, 0, 0, 0);
      goto cleanup;
    }

    memcpy(pBuffer, sounds[i].data, sounds[i].len);
    
    hr = IDirectSoundBuffer_Unlock(pDSB[i], pBuffer, sounds[i].len, 0, 0);
    if(hr != DS_OK)
      goto cleanup;
  }

  return 0;

cleanup:
  sound_exit();
  return -1;
}

int
sound_exit(void)
{
  int i;

  if(pDSB) {
    for(i = 0; i < SOUND_SAMPLES; ++i) {
      if(pDSB[i] != NULL)
        IDirectSoundBuffer_Release(pDSB[i]);
      pDSB[i] = NULL;
    }
    free(pDSB);
    pDSB = NULL;
  }
  if(pDSBChannel) {
    for(i = 0; i < SOUND_CHANNELS; ++i) {
      if(pDSBChannel[i] != NULL) {
        IDirectSoundBuffer_Stop(pDSBChannel[i]);
        IDirectSoundBuffer_Release(pDSBChannel[i]);
      }
      pDSBChannel[i] = NULL;
    }
    free(pDSBChannel);
    pDSBChannel = NULL;
  }
  if(pDS)
    IDirectSound_Release(pDS);
  pDS = NULL;
  
  return 0;
}

int
sound_play(int sound, int channel)
{
  HRESULT hr;

  if(sound<0 || sound>=SOUND_SAMPLES)
    return -1;

  if(channel<0 || channel>=SOUND_CHANNELS)
    return -1;

  sound_stop(channel);

  pDSBChannel[channel] = pDSB[sound];
  IDirectSoundBuffer_AddRef(pDSBChannel[channel]);

  hr = IDirectSoundBuffer_SetCurrentPosition(pDSB[sound], 0);
  hr = IDirectSoundBuffer_Play(pDSB[sound], 0, 0, sound_sounds[sound].loop ? DSBPLAY_LOOPING : 0);

  return hr == DS_OK ? 0 : -1;
}

int
sound_stop(int channel)
{
  if(channel<0 || channel>=SOUND_CHANNELS)
    return -1;

  if(pDSBChannel[channel] != NULL) {
    IDirectSoundBuffer_Stop(pDSBChannel[channel]);
    IDirectSoundBuffer_Release(pDSBChannel[channel]);
    pDSBChannel[channel] = NULL;
  }
  
  return 0;
}

const char *
sound_name(void)
{
  static char name[] = "DirectSound";

  return name;
}
