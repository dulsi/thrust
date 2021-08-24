
/* Written by Peter Ekberg, peda@lysator.liu.se */

/* The code for the extraction of the keys via ioctl's was "stolen" from
   the package kbd-0.92.tar.gz maintained by Andries Brouwer <aeb@cwi.nl>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <vga.h>
#include <vgakeyboard.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <string.h>

#include "compat.h"
#include "thrust_t.h"

#include "keyboard.h"
#include "ksyms.h"

#ifndef HAVE_DEFINE_SCANCODE_P
#define SCANCODE_P 25
#endif

int scancode[5] = {
  SCANCODE_A,
  SCANCODE_S,
  SCANCODE_RIGHTCONTROL,
  SCANCODE_ENTER,
  SCANCODE_SPACE
};

/* All unknown keys are unknown since different keyboards have different
   keys at these scancodes. It would be nice if there were existed some
   library routine to map a scancode to a character. I havn't researched
   this, so this routine might exist without me knowing it. There exists
   a program called getkeycodes that seems to do something like this. */

char *keynames[NR_KEYS];

void
singlekey(void)
{
  keyboard_close();
  vga_unlockvc();
}

void
multiplekeys(void)
{
  vga_lockvc();
  keyboard_init();
}

int
getonemultiplekey(void)
{
  char *keys;
  int i;
  int result=0, gotkey=0;

  keys = keyboard_getstate();

  do {
    usleep(50000L);
    keyboard_update();
    for(i=0; i<NR_KEYS; i++) {
      if(keys[i]) {
	gotkey=i;
	i=NR_KEYS;
      }
    }
  } while(!gotkey);

  do {
    usleep(50000L);
    keyboard_update();
    if(!keys[gotkey])
      result=gotkey;
  } while(!result);

  return(result>=NR_KEYS?0:result);
}

int
getkey(void)
{
  return vga_getkey();
}

ui8
getkeys(void)
{
  ui8 keybits=0;

  keyboard_update();

  if(keyboard_keypressed(SCANCODE_P))
    keybits|=pause_bit;
  else if(keyboard_keypressed(SCANCODE_ESCAPE) ||
	  keyboard_keypressed(SCANCODE_Q))
    keybits|=escape_bit;
  if(keyboard_keypressed(scancode[0]))
    keybits|=left_bit;
  if(keyboard_keypressed(scancode[1]))
    keybits|=right_bit;
  if(keyboard_keypressed(scancode[2]))
    keybits|=thrust_bit;
  if(keyboard_keypressed(scancode[3]))
    keybits|=fire_bit;
  if(keyboard_keypressed(scancode[4]))
    keybits|=pickup_bit;

  return keybits;
}

char *
keystring(int key)
{
  static char keybuffer[100];
  int i;

  if(key<0 || key>=NR_KEYS)
    return NULL;

  strncpy(keybuffer, keynames[key], 99);
  keybuffer[99] = '\0';
  for(i=0; i<strlen(keybuffer); i++)
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
    if(keybuffer[i] == ' ')
      keybuffer[i] = '_';

  for(i=0; i<NR_KEYS; i++) {
    if(!strcasecmp(keynames[i], keybuffer))
      return(i);
  }

  return(0);
}

void
flushkeyboard(void)
{
  keyboard_update();
}

int
keywaiting(void)
{
  return keyboard_update();
}

int
keyinit(void)
{
  struct kbentry keyentry;
  int i, t, v;
  int svgakey_fd;
  char buf[200], *p;

  svgakey_fd = keyboard_init_return_fd();

  if(svgakey_fd == -1)
    return(-1);

  keynames[0] = "Not available";
  for(i=1; i<NR_KEYS; i++) {
    keyentry.kb_table = 0;
    keyentry.kb_index = i;
    if(ioctl(svgakey_fd, KDGKBENT, &keyentry)) {
      perror("Unable to get key value for a keycode");
      return(-1);
    }
    if(keyentry.kb_value == K_NOSUCHMAP) {
      fprintf(stderr, "Strange, no such keyboard map.\n");
      return(-1);
    }
    if(keyentry.kb_value != K_HOLE) {
      t=KTYP(keyentry.kb_value);
      v=KVAL(keyentry.kb_value);
      if(t > KT_LETTER) {
	sprintf(buf, "U+%04x\n", keyentry.kb_value^0xf000);
	p = strdup(buf);
      }
      else {
	if(t == KT_LETTER)
	  t = KT_LATIN;
	if(!(t<syms_size && v<syms[t].size && (p=syms[t].table[v])[0])) {
	  sprintf(buf, "%d\n", keyentry.kb_value);
	  p = strdup(buf);
	}
      }
    }
    else
      p = "Unknown";
    keynames[i] = p;
  }

  /* This is real ugly, but I don't know how else to distinguish them. */
  if(strcasecmp(keynames[29], "Control") == 0)
    keynames[29] = "Control_L";
  if(strcasecmp(keynames[97], "Control") == 0)
    keynames[97] = "Control_R";
  if(strcasecmp(keynames[42], "Shift") == 0)
    keynames[42] = "Shift_L";
  if(strcasecmp(keynames[54], "Shift") == 0)
    keynames[54] = "Shift_R";

  return(0);
}

int
keyclose(void)
{
  keyboard_close();
  return 0;
}

char *
keyname(void)
{
  static char name[] = "SVGA";

  return name;
}
