
/* Written by Dennis Payne, dulsi@identicalsoftware.com */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <SDL.h>

#include "compat.h"
#include "thrust_t.h"
#include "gr_drv.h"

#include "keyboard.h"
#include "init.h"
#include "statistics.h"

int scancode[5] = {
  SDLK_a,
  SDLK_s,
  SDLK_RCTRL,
  SDLK_RETURN,
  SDLK_SPACE
};

ui8 keybits=0;

void
singlekey(void)
{
}

void
multiplekeys(void)
{
  keybits = 0;
}

int
getonemultiplekey(void)
{
  SDL_Event sdlevent;
  int avail;

  for (avail = SDL_WaitEvent(&sdlevent); avail;
    avail = SDL_WaitEvent(&sdlevent))
  {
    switch (sdlevent.type)
    {
      case SDL_QUIT:
      {
        SDL_Quit();
        exit(0);
      }
      case SDL_KEYDOWN:
      {
        return sdlevent.key.keysym.sym;
      }
    }
  }
  return 0;
}

int
getkey(void)
{
  SDL_Event sdlevent;
  int avail;

  displayscreen(0);
  for (avail = SDL_PollEvent(&sdlevent); avail;
    avail = SDL_PollEvent(&sdlevent))
  {
    switch (sdlevent.type)
    {
      case SDL_QUIT:
      {
        SDL_Quit();
        exit(0);
      }
      case SDL_KEYDOWN:
      {
        return sdlevent.key.keysym.sym;
      }
    }
  }
  return 0;
}

ui8
getkeys(void)
{
  SDL_Event sdlevent;
  int avail;

  for (avail = SDL_PollEvent(&sdlevent); avail;
    avail = SDL_PollEvent(&sdlevent))
  {
    switch (sdlevent.type)
    {
      case SDL_QUIT:
      {
        writestatistics();
        restoremem();
        restorehardware();
        exit(0);
      }
      case SDL_KEYDOWN:
      case SDL_KEYUP:
      {
        ui8 bitchange = 0;
        if (sdlevent.key.keysym.sym == SDLK_p)
          bitchange = pause_bit;
        else if (sdlevent.key.keysym.sym == SDLK_ESCAPE)
          bitchange = escape_bit;
        else if (sdlevent.key.keysym.sym == scancode[0])
          bitchange = left_bit;
        else if (sdlevent.key.keysym.sym == scancode[1])
          bitchange = right_bit;
        else if (sdlevent.key.keysym.sym == scancode[2])
          bitchange = thrust_bit;
        else if (sdlevent.key.keysym.sym == scancode[3])
          bitchange = fire_bit;
        else if (sdlevent.key.keysym.sym == scancode[4])
          bitchange = pickup_bit;
        if (sdlevent.type == SDL_KEYDOWN)
          keybits |= bitchange;
        else
          keybits &= ~bitchange;
      }
    }
  }

  return keybits;
}

char *
keystring(int key)
{
  static char keybuffer[100];
  strcpy(keybuffer, SDL_GetKeyName(key));
  for(int i=0; i<strlen(keybuffer); i++)
    if(keybuffer[i] == '_')
      keybuffer[i] = ' ';

  return keybuffer;
}

int
keycode(char *keyname)
{
  static char keybuffer[100];
  int i;

  strncpy(keybuffer, keyname, 99);
  keybuffer[99] = '\0';
  for(i=0; i<strlen(keybuffer); i++)
    if(keybuffer[i] == '_')
      keybuffer[i] = ' ';

  return SDL_GetKeyFromName(keybuffer);
}

void
flushkeyboard(void)
{
  SDL_Event sdlevent;
  int avail;

  for (avail = SDL_PollEvent(&sdlevent); avail;
    avail = SDL_PollEvent(&sdlevent))
  {
  }
}

int
keywaiting(void)
{
//  return keyboard_update();
  return 0;
}

int
keyinit(void)
{
  if (SDL_Init(SDL_INIT_EVENTS) < 0)
  {
    printf("Failed - SDL_Init\n");
    exit(0);
  }

  return(0);
}

int
keyclose(void)
{
  return 0;
}

char *
keyname(void)
{
  static char name[] = "SDL";

  return name;
}
