
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <ggi/ggi.h>
#include <ggi/keyboard.h>
#include <string.h>

#include "thrust_t.h"
#include "keyboard.h"

extern ggi_visual_t vis;
const char *giik2str(uint32_t giik, int issym);

int scancode[8] = {
  'A',
  'S',
  GIIK_CtrlR,
  GIIK_Enter,
  GIIUC_Space,
  'P',
  GIIUC_Escape,
  'Q'
};

static gii_event_mask mask = emCommand | emInformation | emExpose | emKey;
static ui8 keybits;

void
singlekey(void)
{
  flushkeyboard();
}

void
multiplekeys(void)
{
  keybits = 0;
}

int
getonemultiplekey(void)
{
  gii_event ev;

  while(1) {
    ggiEventRead(vis, &ev, mask);
    if(ev.any.type == evKeyRelease) {
      if(ev.key.label == GIIK_VOID)
	return (ev.key.origin << 17) | 0x10000 | ev.key.button;
      else
	return ev.key.label;
    }
  }

  return 0;
}

int
getkey(void)
{
  gii_event ev;
  struct timeval tv;
  int events;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  ggiEventPoll(vis, mask, &tv);
  events = ggiEventsQueued(vis, mask);
  while(events--) {
    ggiEventRead(vis, &ev, mask);
    if(ev.any.type != evKeyRelease)
      continue;

    if(GII_KTYP(ev.key.label) == GII_KT_MOD)
      continue;

    return ev.key.sym;
  }

  return 0;
}

ui8
getkeys(void)
{
  gii_event ev;
  struct timeval tv;
  int events;
  int keycode;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  ggiEventPoll(vis, mask, &tv);
  events = ggiEventsQueued(vis, mask);
  while(events--) {
    ggiEventRead(vis, &ev, mask);
    if(ev.key.label == GIIK_VOID)
      keycode = (ev.key.origin << 17) | 0x10000 | ev.key.button;
    else
      keycode = ev.key.label;
    switch(ev.any.type) {
    case evKeyPress:
    case evKeyRepeat:
    case evKeyRelease:
      if(keycode == scancode[0])
	keybits = (keybits & ~left_bit)
	| (ev.any.type==evKeyRelease?0:left_bit);
      if(keycode == scancode[1])
	keybits = (keybits & ~right_bit)
	| (ev.any.type==evKeyRelease?0:right_bit);
      if(keycode == scancode[2])
	keybits = (keybits & ~thrust_bit)
	| (ev.any.type==evKeyRelease?0:thrust_bit);
      if(keycode == scancode[3])
	keybits = (keybits & ~fire_bit)
	| (ev.any.type==evKeyRelease?0:fire_bit);
      if(keycode == scancode[4])
	keybits = (keybits & ~pickup_bit)
	| (ev.any.type==evKeyRelease?0:pickup_bit);
      if(keycode == scancode[5])
	keybits = (keybits & ~pause_bit)
	| (ev.any.type==evKeyRelease?0:pause_bit);
      if(keycode == scancode[6]
	 || ev.key.label == scancode[7])
	keybits = (keybits & ~escape_bit)
	| (ev.any.type==evKeyRelease?0:escape_bit);
      break;
    /*
    case FocusOut:
      in_focus = 0;
      nodemo = 1;
      keybits = (keybits & ~pause_bit) | pause_bit;
      singlekey();
      break;
    case FocusIn:
      in_focus = 1;
      nodemo = real_nodemo;
      multiplekeys();
      break;
    case ClientMessage:
      if(event.xclient.data.l[0] == XInternAtom(dp, "WM_DELETE_WINDOW", False))
	raise(SIGTERM);
      break;
    */
    }
  }

  return keybits;
}

char *
keystring(int key)
{
  static char keybuffer[100];
  int i;

  if(key & 0x10000)
    sprintf(keybuffer, "%d", key);
  else {
    strncpy(keybuffer, giik2str(key, 0), 99);
    keybuffer[99] = '\0';
  }
  for(i=0; i<strlen(keybuffer); i++)
    if(keybuffer[i] == '_')
      keybuffer[i] = ' ';
  if(!keybuffer[1])
    keybuffer[0] = toupper(keybuffer[0]);
  return keybuffer;
}

int
keycode(char *keyname)
{
  static uint32_t page[] = {
    GII_KT_LATIN1,
    GII_KT_SPEC,
    GII_KT_FN,
    GII_KT_PAD,
    GII_KT_MOD,
    GII_KT_DEAD
  };
  static char keybuffer[100];
  char *key;
  int i, j;

  strncpy(keybuffer, keyname, 99);
  keybuffer[99] = '\0';
  for(i=0; i<strlen(keybuffer); i++)
    if(keybuffer[i] == ' ')
      keybuffer[i] = '_';

  for(j=0; j<sizeof(page)/sizeof(page[0]); ++j) {
    for(i=0; i<256; i++) {
      key = keystring(GII_KEY(page[j], i));
      if(key[1] == '\0') {
	if(!strcmp(key, keybuffer))
	  return GII_KEY(page[j], i);
      }
      else if(!strcasecmp(key, keybuffer))
	return GII_KEY(page[j], i);
    }
  }

  return atoi(keyname);
}

void
flushkeyboard(void)
{
  gii_event ev;
  struct timeval tv;
  int events;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  ggiEventPoll(vis, mask, &tv);
  events = ggiEventsQueued(vis, emKey);
  while(events--)
    ggiEventRead(vis, &ev, emKey);
}

int
keywaiting(void)
{
  struct timeval tv;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  ggiEventPoll(vis, mask, &tv);
  return ggiEventsQueued(vis, emKeyPress | emKeyRepeat) ? 1 : 0;
}

int
keyinit(void)
{
  if(ggiSetEventMask(vis, mask)) {
    printf("Unable to set input event mask.\n");
    return(-1);
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
  static char name[] = "GGI";

  return name;
}
