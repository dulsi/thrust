
/* Written by Peter Ekberg, peda@lysator.liu.se, after
   a long look at the source code for Koules version 1.3.
   Koules is also released under GPL with Copyrights owned
   by Jan Hubicka, hubicka@limax.paru.cas.cz. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <string.h>
#include <X11/Xlib.h>
#include <sys/signal.h>

#include "thrust_t.h"
#include "keyboard.h"
#include "X11.h"

int scancode[8] = { 0, 0, 0, 0,
                    0, 0, 0, 0 };

extern Display *dp;
extern Window wi;
extern int nodemo;

XKeyboardControl keyboard_control;
ui8 keybits=0;

int in_focus=0;
int real_nodemo;

void
singlekey(void)
{
  keyboard_control.auto_repeat_mode = AutoRepeatModeDefault;
  XChangeKeyboardControl(dp, KBAutoRepeatMode, &keyboard_control);
}

void
multiplekeys(void)
{
  XAutoRepeatOff(dp);
  keybits = 0;
}

int
getonemultiplekey(void)
{
  int gotkey=0;
  XEvent event;

  XSync(dp, False);
  do {
    XNextEvent(dp, &event);
    switch(event.type) {
    case KeyRelease:
      gotkey=event.xkey.keycode;
      break;
    case FocusOut:
      in_focus = 0;
      nodemo = 1;
      singlekey();
      break;
    case FocusIn:
      in_focus = 1;
      nodemo = real_nodemo;
      multiplekeys();
      break;
    case VisibilityNotify:
      CopyToScreen();
      break;
    case ClientMessage:
      if(event.xclient.data.l[0] == XInternAtom(dp, "WM_DELETE_WINDOW", False))
	raise(SIGTERM);
    }
  } while(!gotkey);

  return gotkey;
}

int
getkey(void)
{
  int key=0;
  XEvent event;
  unsigned int mask;

  XSync(dp, False);
  while(XEventsQueued(dp, QueuedAlready)) {
    XNextEvent(dp, &event);
    switch(event.type) {
    case KeyRelease:
      {
	Window root, child;
	int x, y, winx, winy;

	XQueryPointer(dp, wi, &root, &child, &x, &y, &winx, &winy, &mask);
      }
      key = XLookupKeysym(&event.xkey, mask);
      switch(key) {
      case XK_BackSpace:
      case XK_Delete:
	key = 127;
	break;
      case XK_Return:
      case XK_Linefeed:
	key = 10;
	break;
      case XK_Escape:
	key = 27;
	break;
      }
      break;
    case KeyPress:
      break;
    case FocusOut:
      in_focus = 0;
      nodemo = 1;
      singlekey();
      break;
    case FocusIn:
      in_focus = 1;
      nodemo = real_nodemo;
      break;
    case VisibilityNotify:
      CopyToScreen();
      break;
    case ClientMessage:
      if(event.xclient.data.l[0] == XInternAtom(dp, "WM_DELETE_WINDOW", False))
	raise(SIGTERM);
      break;
    }
  }

  return((key<0 || key>=256) ? 0 : key);
}

ui8
getkeys(void)
{
  XEvent event;

  XSync(dp, False);
  while(XEventsQueued(dp, QueuedAlready)) {
    XNextEvent(dp, &event);
    switch(event.type) {
    case KeyRelease:
    case KeyPress:
      if(event.xkey.keycode == scancode[0])
	keybits = (keybits & ~left_bit)
	| (event.type==KeyPress?left_bit:0);
      if(event.xkey.keycode == scancode[1])
	keybits = (keybits & ~right_bit)
	| (event.type==KeyPress?right_bit:0);
      if(event.xkey.keycode == scancode[2])
	keybits = (keybits & ~thrust_bit)
	| (event.type==KeyPress?thrust_bit:0);
      if(event.xkey.keycode == scancode[3])
	keybits = (keybits & ~fire_bit)
	| (event.type==KeyPress?fire_bit:0);
      if(event.xkey.keycode == scancode[4])
	keybits = (keybits & ~pickup_bit)
	| (event.type==KeyPress?pickup_bit:0);
      if(event.xkey.keycode == scancode[5])
	keybits = (keybits & ~pause_bit)
	| (event.type==KeyPress?pause_bit:0);
      if(event.xkey.keycode == scancode[6]
	 || event.xkey.keycode == scancode[7])
	keybits = (keybits & ~escape_bit)
	| (event.type==KeyPress?escape_bit:0);
      break;
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
    }
  }

  return keybits;
}

char *
keystring(int key)
{
  static char keybuffer[100];
  int i;
  KeySym sym;

  sym = XKeycodeToKeysym(dp, key, 0);
  if(sym != NoSymbol)
    strncpy(keybuffer, XKeysymToString(sym), 99);
  else
    strcpy(keybuffer, "Unknown");
  keybuffer[99] = '\0';
  for(i=0; i<strlen(keybuffer); i++)
    if(keybuffer[i] == '_')
      keybuffer[i] = ' ';

  return(keybuffer);
}

int
keycode(char *keyname)
{
  static char keybuffer[100];
  int i;
  KeySym sym;

  strncpy(keybuffer, keyname, 99);
  for(i=0; i<strlen(keybuffer); i++) {
    if(keybuffer[i] == ' ')
      keybuffer[i] = '_';
  }

  sym = XStringToKeysym(keyname);

  if(sym == NoSymbol)
    return(0);

  return(XKeysymToKeycode(dp, sym));
 }

void
flushkeyboard(void)
{
  XEvent event;
  Bool res;

  XSync(dp, False);

  do
    res = XCheckMaskEvent(dp, KeyPressMask | KeyReleaseMask, &event);
  while(res);
}

int
keywaiting(void)
{
  XEvent event;
  Bool res;

  do {
    XSync(dp, False);

    if(XCheckMaskEvent(dp, FocusChangeMask | VisibilityChangeMask, &event)) {
      switch(event.type) {
      case FocusOut:
	in_focus = 0;
	nodemo = 1;
	singlekey();
	break;
      case FocusIn:
	in_focus = 1;
	nodemo = real_nodemo;
	multiplekeys();
	break;
      case VisibilityNotify:
	CopyToScreen();
	break;
      }
    }
    if(XCheckTypedWindowEvent(dp, wi, ClientMessage, &event)) {
      if(event.xclient.data.l[0]
	 == XInternAtom(dp, "WM_DELETE_WINDOW", False))
	raise(SIGTERM);
    }
  } while(!in_focus);

  do
    res = XCheckMaskEvent(dp, KeyPressMask | KeyReleaseMask, &event);
  while(res && event.type==KeyPress);

  if(res) {
    XPutBackEvent(dp, &event);
    return 1;
  }
  else
    return 0;
}

int
keyinit(void)
{
  static KeySym keys[8] = {
    XK_A, XK_S, XK_Control_R, XK_Return, XK_space, XK_P, XK_Escape, XK_Q };
  int i;

  real_nodemo = nodemo;

  for(i=0; i<8; i++) {
    if(scancode[i] != 0)
      i=9;
  }
  if(i==8) {
    for(i=0; i<8; i++)
      scancode[i] = XKeysymToKeycode(dp, keys[i]);
  }

  multiplekeys();
  return 0;
}

int
keyclose(void)
{
  singlekey();
  return 0;
}

char *
keyname(void)
{
  static char name[] = "X11";

  return name;
}
