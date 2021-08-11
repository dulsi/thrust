
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <dpmi.h>
#include <go32.h>

#include "keyboard.h"
#include "doskey.h"

static _go32_dpmi_seginfo wrapper;
static __dpmi_paddr single_handler, multi_handler;
static int keys[128];

int scancode[5] = {
  SCANCODE_A,
  SCANCODE_S,
  SCANCODE_RIGHTSHIFT,
  SCANCODE_ENTER,
  SCANCODE_SPACE
};

char *keynames[128] = {
  "Not_Available",
  "Escape",            /*   1 */
  "1",                 /*   2 */
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "0",                 /*  11 */
  "Minus",
  "Equal",
  "Backspace",         /*  14 */
  "Tab",               /*  15 */
  "Q",                 /*  16 */
  "W",
  "E",
  "R",
  "T",
  "Y",
  "U",
  "I",
  "O",
  "P",                 /*  25 */
  "Bracket_Left",
  "Bracket_Right",
  "Enter",             /*  28 */
  "Left_Control",      /*  29 */
  "A",                 /*  30 */
  "S",
  "D",
  "F",
  "G",
  "H",
  "J",
  "K",
  "L",                 /*  38 */
  "Semicolon",
  "Apostrophe",
  "Grave",
  "Left_Shift",        /*  42 */
  "Backslash",
  "Z",                 /*  44 */
  "X",
  "C",
  "V",
  "B",
  "N",
  "M",                 /*  50 */
  "Comma",
  "Period",
  "Slash",
  "Right_Shift",       /*  54 */
  "Keypad_Multiply",
  "Left_Alt",          /*  56 */
  "Space",             /*  57 */
  "Caps_Lock",         /*  58 */
  "F1",                /*  59 */
  "F2",
  "F3",
  "F4",
  "F5",
  "F6",
  "F7",
  "F8",
  "F9",
  "F10",               /*  68 */
  "Num_Lock",          /*  69 */
  "Scroll_Lock",       /*  70 */
  "Keypad_Home",       /*  71 */
  "Keypad_Up",
  "Keypad_PgUp",       /*  73 */
  "Keypad_Minus",
  "Keypad_Left",       /*  75 */
  "Keypad 5",
  "Keypad_Right",      /*  77 */
  "Keypad_Plus",
  "Keypad_End",        /*  79 */
  "Keypad_Down",
  "Keypad_PgDn",       /*  81 */
  "Keypad_0",          /*  82 */
  "Keypad_Period",     /*  83 */
  "Unknown",
  "Unknown",
  "Less",
  "F11",               /*  87 */
  "F12",               /*  88 */
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Keypad_Enter",      /*  96 */
  "Right_Control",     /*  97 */
  "Keypad_Divide",
  "Print_Screen",      /*  99 */
  "Right_Alt",         /* 100 */
  "Break",
  "Home",              /* 102 */
  "Cursor_Up",         /* 103 */
  "Page_Up",           /* 104 */
  "Cursor_Left",       /* 105 */
  "Cursor_Right",      /* 106 */
  "End",               /* 107 */
  "Cursor_Down",       /* 108 */
  "Page_Down",         /* 109 */
  "Insert",            /* 110 */
  "Delete",            /* 111 */
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Break_Alternative", /* 119 */
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown"
};


static inline void port_out(int value, int port)
{
    __asm__ volatile ("outb %0,%1"
	      ::"a" ((ui8) value), "d"((unsigned short) port));
}

static inline int port_in(int port)
{
    ui8 value;
    __asm__ volatile ("inb %1,%0"
		      :"=a" (value)
		      :"d"((unsigned short) port));
    return value;
}

void
keyintr(void)
{
  int tmp;

  tmp = port_in(0x60);
  keys[tmp&0x7f] = !(tmp&0x80);
  port_out(0x20, 0x20);
}

int
pindown_mem(void *ptr, unsigned long bytes)
{
  unsigned long base;

  if(__dpmi_get_segment_base_address(_my_ds(), &base) != -1) {
    __dpmi_meminfo mem;

    mem.handle  = 0;
    mem.size    = bytes;
    mem.address = (unsigned long)((char *)ptr + base);
    if(__dpmi_lock_linear_region(&mem) != -1)
      return(1);
  }

  return(-1);
}

void
test_keycomb(void)
{
  if((keys[SCANCODE_LEFTCONTROL] || keys[SCANCODE_RIGHTCONTROL])
     && keys[SCANCODE_C])
    raise(SIGINT);
}


void
singlekey(void)
{
  __dpmi_get_and_disable_virtual_interrupt_state();
  __dpmi_set_protected_mode_interrupt_vector(9, &single_handler);
  __dpmi_get_and_enable_virtual_interrupt_state();  
}

void
multiplekeys(void)
{
  int i;

  for(i=0; i<128; i++)
    keys[i] = 0;

  __dpmi_get_and_disable_virtual_interrupt_state();
  __dpmi_set_protected_mode_interrupt_vector(9, &multi_handler);
  __dpmi_get_and_enable_virtual_interrupt_state();  
}

int
getonemultiplekey(void)
{
  int i;
  int gotkey=0;

  test_keycomb();

  do {
    usleep(50000L);
    for(i=1; i<128; i++) {
      if(keys[i]) {
	gotkey=i;
	i=128;
      }
    }
  } while(!gotkey);

  while(keys[gotkey])
    usleep(50000L);

  return(gotkey>=128 ? 0 : gotkey);
}

int
getkey(void)
{
  int ch;

  if(kbhit()) {
    ch = getch();
    if(!ch)
      return(0x100 + getch());
    return(ch);
  }
  return(0);
}

ui8
getkeys(void)
{
  ui8 keybits=0;

  test_keycomb();

  if(keys[SCANCODE_P])
    keybits|=pause_bit;
  else if(keys[SCANCODE_ESCAPE] ||
	  keys[SCANCODE_Q])
    keybits|=escape_bit;
  if(keys[scancode[0]])
    keybits|=left_bit;
  if(keys[scancode[1]])
    keybits|=right_bit;
  if(keys[scancode[2]])
    keybits|=thrust_bit;
  if(keys[scancode[3]])
    keybits|=fire_bit;
  if(keys[scancode[4]])
    keybits|=pickup_bit;

  return keybits;
}

char *
keystring(int key)
{
  static char keybuffer[100];
  int i;

  if(key<0 || key>=128)
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

  for(i=0; i<128; i++)
    if(!strcasecmp(keynames[i], keybuffer))
      return(i);

  return(0);
}

void
flushkeyboard(void)
{
  test_keycomb();
}

int
keywaiting(void)
{
  int i;

  test_keycomb();

  for(i=1; i<128; i++)
    if(keys[i])
      return(1);

  return(0);
}

int
keyinit(void)
{
  wrapper.pm_offset   = (int)keyintr;
  wrapper.pm_selector = _my_cs();
  _go32_dpmi_allocate_iret_wrapper(&wrapper);

  multi_handler.offset32 = wrapper.pm_offset;
  multi_handler.selector = wrapper.pm_selector;

  if(pindown_mem(keyintr, (long)test_keycomb - (long)keyintr) == -1) {
    fprintf(stderr, "Unable to lock keyboard interrupt handler in memory.\n");
    return(-1);
  }

  __dpmi_get_protected_mode_interrupt_vector(9, &single_handler);

  multiplekeys();

  return(0);
}

int
keyclose(void)
{
  singlekey();
  return(0);
}

char *
keyname(void)
{
  static char name[] = "MSDOG";

  return name;
}
