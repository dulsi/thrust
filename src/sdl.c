
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
  ui8 *tmp;
  int res=PSTARTX+destx+320*(PSTARTY+desty);
  int dy, seg, lastseg, endseg;

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
  int used_first;
  int used_entries=0;

  entries=last-first+1;
  
  used_first = color_lookup[first];
  if(used_first == 0xff)
    printf("Aiee, bad fadepalette call.\n");
  c=&tmpRGBtable[3*used_first];
  d=RGBtable;
  i=0;

  while(i<entries) {
    if(color_lookup[first+i] != 0xff) {
      *c++ = (*d++ * fade) >> 8;
      *c++ = (*d++ * fade) >> 8;
      *c = (*d * fade) >> 8;
      used_entries++;
    }
    else
      d += 2;
    if(++i<entries) {
      if(color_lookup[first+i-1] != 0xff)
        c++;
      d++;
    }
  }

//  if(flag)
//    vga_waitretrace();

  SDL_Color sdlcol[256];
  int color, rgb;

  for (color = 0; color < 256; color++)
  {
    sdlcol[color].r = tmpRGBtable[color * 3] << 2;
    sdlcol[color].g = tmpRGBtable[color* 3 + 1] << 2;
    sdlcol[color].b = tmpRGBtable[color * 3 + 2] << 2;
  }
  SDL_SetPaletteColors((SDL_Palette *)IPaletteMain, sdlcol, used_first, used_entries);
}

void
fade_in(unsigned long us)
{
  int i;

/*  if(PSTARTY>0 && PSTARTX>0) {
    gl_hline(PSTARTX-3, PSTARTY-3, PSTARTX+PUSEX+2, 1);
    gl_hline(PSTARTX-4, PSTARTY-4, PSTARTX+PUSEX+3, 2);
    gl_hline(PSTARTX-5, PSTARTY-5, PSTARTX+PUSEX+4, 3);
    gl_hline(PSTARTX-3, PSTARTY+PUSEY+26, PSTARTX+PUSEX+2, 3);
    gl_hline(PSTARTX-4, PSTARTY+PUSEY+27, PSTARTX+PUSEX+3, 2);
    gl_hline(PSTARTX-5, PSTARTY+PUSEY+28, PSTARTX+PUSEX+4, 1);
    gl_line(PSTARTX-3, PSTARTY-3, PSTARTX-3, PSTARTY+PUSEY+26, 1);
    gl_line(PSTARTX-4, PSTARTY-4, PSTARTX-4, PSTARTY+PUSEY+27, 2);
    gl_line(PSTARTX-5, PSTARTY-5, PSTARTX-5, PSTARTY+PUSEY+28, 3);
    gl_line(PSTARTX+PUSEX+2, PSTARTY-3, PSTARTX+PUSEX+2, PSTARTY+PUSEY+26, 3);
    gl_line(PSTARTX+PUSEX+3, PSTARTY-4, PSTARTX+PUSEX+3, PSTARTY+PUSEY+27, 2);
    gl_line(PSTARTX+PUSEX+4, PSTARTY-5, PSTARTX+PUSEX+4, PSTARTY+PUSEY+28, 1);
  }
  else if(PSTARTY>0) {
    gl_hline(PSTARTX, PSTARTY-3, PSTARTX+PUSEX-1, 1);
    gl_hline(PSTARTX, PSTARTY-4, PSTARTX+PUSEX-1, 2);
    gl_hline(PSTARTX, PSTARTY-5, PSTARTX+PUSEX-1, 3);
    gl_hline(PSTARTX, PSTARTY+PUSEY+26, PSTARTX+PUSEX-1, 3);
    gl_hline(PSTARTX, PSTARTY+PUSEY+27, PSTARTX+PUSEX-1, 2);
    gl_hline(PSTARTX, PSTARTY+PUSEY+28, PSTARTX+PUSEX-1, 1);
  }*/

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
