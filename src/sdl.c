
/* Written by Dennis Payne, dulsi@identicalsoftware.com */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#include "compat.h"
#include "thrust.h"
#include "fast_gr.h"
#include "gr_drv.h"
#include "options.h"

#define VGAMODE G320x200x256
#define PSTARTX ((320-PUSEX)>>1)
#define PSTARTY ((200-PUSEY-24)>>1)

typedef unsigned char IColor;

typedef IColor IPaletteTable[256][3];
typedef IPaletteTable *IPalette;

/* Extra global variables needed by SDL */
SDL_Window *ISDLMainWindow;
SDL_Renderer *ISDLMainRenderer;
SDL_Texture *ISDLMainTexture;
SDL_Surface *ISDLMainScreen;
SDL_Surface *ISDLScreen;
unsigned char *IScreenMain;
IPalette IPaletteMain;

void
clearscr(void)
{
//  SDL_RenderClear(ISDLMainRenderer);
  memset(IScreenMain, 0, 320 * 200);
}

void
putarea(ui8 *source,
	int x, int y, int width, int height, int bytesperline,
	int destx, int desty)
{
  if(bytesperline==320 && width==320 && 320==320 && x==0 && destx==0) {
    memcpy(IScreenMain + desty*320, source + y*320, height*320);
  }
  else {
    for (int cy = 0; cy < height; cy++)
    {
      if (desty + cy >= 200)
        break;
      if (destx + width <= 320)
        memcpy(IScreenMain + (desty + cy) * 320 + destx, source + (y + cy) * bytesperline + x, width);
      else
        memcpy(IScreenMain + (desty + cy) * 320 + destx, source + (y + cy) * bytesperline + x, 320 - destx);
    }
  }
}

void
putpixel(int x, int y, ui8 color)
{
  IScreenMain[PSTARTX+x + ((PSTARTY+y) << 8) + ((PSTARTY+y) << 6)] = color;
}

static void
delaygame(long usec)
{
  static long extratime[3] = { 0, 0, 0 };
  struct timeval start, end, diff;
  long extradelay, delay;
  int i;

  extradelay = (extratime[0] + extratime[1] + extratime[2])/3;
  if(usec > extradelay) {
    gettimeofday(&start, NULL);
    usleep(usec - extradelay);
    gettimeofday(&end, NULL);
    diff.tv_sec = end.tv_sec - start.tv_sec;
    diff.tv_usec = end.tv_usec - start.tv_usec;
    if(diff.tv_usec < 0) {
      diff.tv_sec--;
      diff.tv_usec += 1000000;
    }
    delay = diff.tv_sec*1000000 + diff.tv_usec;
    extratime[0] = extratime[1];
    extratime[1] = extratime[2];
    extratime[2] = delay - (usec - extradelay);
  }
  else
    for(i=0; i<3; i++)
      extratime[i] = (extratime[i] * 9) / 10;
}

void
syncscreen(unsigned long us)
{
  struct timeval end, diff;
  static struct timeval start;
  static int first = 1;
  long delay;

  if(us)
    usleep(us);

  if(first) {
    gettimeofday(&start, NULL);
    first = 0;
  }

  gettimeofday(&end, NULL);
  if(end.tv_sec < start.tv_sec ||
     (end.tv_sec == start.tv_sec &&
      end.tv_usec < start.tv_usec))
    end.tv_sec += 60*60*24;
  diff.tv_sec = end.tv_sec - start.tv_sec;
  diff.tv_usec = end.tv_usec - start.tv_usec;
  if(diff.tv_usec < 0) {
    diff.tv_sec--;
    diff.tv_usec += 1000000;
  }

  delay = 20000 - (diff.tv_sec*1000000 + diff.tv_usec);
  if(delay > 0) {
    delaygame(delay);
  }
  gettimeofday(&start, NULL);
}

void
displayscreen(unsigned long us)
{
  void *pixels;
  int pitch;
  SDL_Rect dest;
  dest.x = 0;
  dest.y = 0;
  dest.w = 320;
  dest.h = 200;
  SDL_LockTexture(ISDLMainTexture, &dest, &pixels, &pitch);
  int x, y;
  unsigned char *curPos;
  Uint8 *realLine;
  Uint8 *realPos;

  curPos = IScreenMain;
  realPos = (Uint8 *)pixels;
  for (y = 0; y < 200; ++y)
  {
    realLine = realPos;
    for (x = 0; x < 320; ++x)
    {
      SDL_GetRGB(*curPos, ISDLScreen->format, realPos + 2, realPos + 1, realPos);
      realPos += 4;
      ++curPos;
    }
    realPos = realLine + pitch;
  }
  SDL_UnlockTexture(ISDLMainTexture);
  SDL_RenderClear(ISDLMainRenderer);
  SDL_RenderCopy(ISDLMainRenderer, ISDLMainTexture, NULL, NULL);
  SDL_RenderPresent(ISDLMainRenderer);
  if(us)
    usleep(us);
}

void
fadepalette(int first, int last, ui8 *RGBtable, int fade, int flag)
{
  static int tmpRGBtable[768];
  int entries,i;
  int *c;
  ui8 *d;

  entries=last-first+1;

  c=tmpRGBtable;
  d=RGBtable;
  i=0;

  while(i<entries) {
    *c++ = (*d++ * fade) >> 8;
    *c++ = (*d++ * fade) >> 8;
    *c = (*d * fade) >> 8;
    i++;
  }

  if(flag)
    displayscreen(0);

  SDL_Color sdlcol[256];
  int color;

  for (color = 0; color < 256; color++)
  {
    sdlcol[color].r = tmpRGBtable[color * 3] << 2;
    sdlcol[color].g = tmpRGBtable[color* 3 + 1] << 2;
    sdlcol[color].b = tmpRGBtable[color * 3 + 2] << 2;
  }
  SDL_SetPaletteColors((SDL_Palette *)IPaletteMain, sdlcol, 0, 256);
}

void
fade_in(unsigned long us)
{
  int i;

  for(i=1; i<=64; i++)
    fadepalette(0, 255, bin_colors, i, 1);

  if(us)
    usleep(us);
}

void
fade_out(unsigned long us)
{
  int i;

  if(us)
    usleep(us);

  for(i=64; i; i--)
    fadepalette(0, 255, bin_colors, i, 1);
  clearscr();
  usleep(500000L);
}

void
graphics_preinit(void)
{
}

int
graphicsinit(int argc, char **argv)
{
  int optc;
  int windowed = 0;

  optind=0;
  do {
    static struct option longopts[] = {
      OPTS,
      SDL_OPTS,
      { 0, 0, 0, 0 }
    };

    optc=getopt_long_only(argc, argv, OPTC SDL_OPTC, longopts, (int *)0);
    switch(optc) {
    case 'w':
      windowed = 1;
      break;
    }
  } while(optc != EOF);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    printf("Failed - SDL_Init\n");
    exit(0);
  }
  ISDLMainWindow = SDL_CreateWindow("Thrust",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    320, 200,
    (!windowed ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
  if (ISDLMainWindow == NULL)
  {
    printf("Failed - SDL_CreateWindow\n");
    exit(0);
  }
  ISDLMainRenderer = SDL_CreateRenderer(ISDLMainWindow, -1, 0);
  if (ISDLMainRenderer == NULL)
  {
    printf("Failed - SDL_CreateRenderer\n");
    exit(0);
  }
  ISDLMainTexture = SDL_CreateTexture(ISDLMainRenderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    320, 200);
  if (ISDLMainTexture == NULL)
  {
    printf("Failed - SDL_CreateTexture\n");
    exit(0);
  }
  ISDLMainScreen = SDL_CreateRGBSurface(0, 320, 200, 32,
    0x00FF0000,
    0x0000FF00,
    0x000000FF,
    0xFF000000);
  ISDLScreen = SDL_CreateRGBSurface(0, 320, 200, 8, 0, 0, 0, 0);
  if (ISDLScreen == NULL)
  {
    printf("Failed - SDL_CreateRGBSurface\n");
    exit(0);
  }
  IScreenMain = (unsigned char *)ISDLScreen->pixels;
  IPaletteMain = (IPalette)ISDLScreen->format->palette;

  SDL_RenderClear(ISDLMainRenderer);
  fadepalette(0, 255, bin_colors, 1, 0);

  return 0;
}

int
graphicsclose(void)
{
  SDL_Quit();

  return 0;
}

char *
graphicsname(void)
{
  static char name[] = "SDL";

  return name;
}
