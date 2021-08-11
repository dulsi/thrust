
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef KEYBOARD_H
#define KEYBOARD_H

enum {
  pause_bit =1<<0,
  escape_bit=1<<1,
  right_bit =1<<2,
  left_bit  =1<<3,
  fire_bit  =1<<4,
  pickup_bit=1<<5,
  thrust_bit=1<<6,
  quit_bit  =1<<7
};

#ifdef __STDC__
void singlekey(void);
void multiplekeys(void);
int getonemultiplekey(void);
int getkey(void);
ui8 getkeys(void);
char *keystring(int key);
int keycode(char *keyname);
void flushkeyboard(void);
int keywaiting(void);
int keyinit(void);
int keyclose(void);
char *keyname(void);
#endif

#endif
