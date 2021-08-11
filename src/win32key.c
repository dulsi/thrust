
/* Written by Peter Ekberg, peda@lysator.liu.se. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <windows.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#include "thrust_t.h"
#include "keyboard.h"
#include "font5x5.h"

#define SCANCODE_ESCAPE			1

#define SCANCODE_1			2
#define SCANCODE_2			3
#define SCANCODE_3			4
#define SCANCODE_4			5
#define SCANCODE_5			6
#define SCANCODE_6			7
#define SCANCODE_7			8
#define SCANCODE_8			9
#define SCANCODE_9			10
#define SCANCODE_0			11

#define SCANCODE_MINUS			12
#define SCANCODE_EQUAL			13

#define SCANCODE_BACKSPACE		14
#define SCANCODE_TAB			15

#define SCANCODE_Q			16
#define SCANCODE_W			17
#define SCANCODE_E			18
#define SCANCODE_R			19
#define SCANCODE_T			20
#define SCANCODE_Y			21
#define SCANCODE_U			22
#define SCANCODE_I			23
#define SCANCODE_O			24
#define SCANCODE_P			25
#define SCANCODE_BRACKET_LEFT		26
#define SCANCODE_BRACKET_RIGHT		27

#define SCANCODE_ENTER			28

#define SCANCODE_LEFTCONTROL		29

#define SCANCODE_A			30
#define SCANCODE_S			31
#define SCANCODE_D			32
#define SCANCODE_F			33
#define SCANCODE_G			34
#define SCANCODE_H			35
#define SCANCODE_J			36
#define SCANCODE_K			37
#define SCANCODE_L			38
#define SCANCODE_SEMICOLON		39
#define SCANCODE_APOSTROPHE		40
#define SCANCODE_GRAVE			41

#define SCANCODE_LEFTSHIFT		42
#define SCANCODE_BACKSLASH		43

#define SCANCODE_Z			44
#define SCANCODE_X			45
#define SCANCODE_C			46
#define SCANCODE_V			47
#define SCANCODE_B			48
#define SCANCODE_N			49
#define SCANCODE_M			50
#define SCANCODE_COMMA			51
#define SCANCODE_PERIOD			52
#define SCANCODE_SLASH			53

#define SCANCODE_RIGHTSHIFT		54
#define SCANCODE_KEYPADMULTIPLY		55

#define SCANCODE_LEFTALT		56
#define SCANCODE_SPACE			57
#define SCANCODE_CAPSLOCK		58

#define SCANCODE_F1			59
#define SCANCODE_F2			60
#define SCANCODE_F3			61
#define SCANCODE_F4			62
#define SCANCODE_F5			63
#define SCANCODE_F6			64
#define SCANCODE_F7			65
#define SCANCODE_F8			66
#define SCANCODE_F9			67
#define SCANCODE_F10			68

#define SCANCODE_PAUSE                  69
#define SCANCODE_SCROLLLOCK		70

#define SCANCODE_KEYPAD7		71
#define SCANCODE_CURSORUPLEFT		71
#define SCANCODE_KEYPAD8		72
#define SCANCODE_CURSORUP		72
#define SCANCODE_KEYPAD9		73
#define SCANCODE_CURSORUPRIGHT		73
#define SCANCODE_KEYPADMINUS		74
#define SCANCODE_KEYPAD4		75
#define SCANCODE_CURSORLEFT		75
#define SCANCODE_KEYPAD5		76
#define SCANCODE_KEYPAD6		77
#define SCANCODE_CURSORRIGHT		77
#define SCANCODE_KEYPADPLUS		78
#define SCANCODE_KEYPAD1		79
#define SCANCODE_CURSORDOWNLEFT		79
#define SCANCODE_KEYPAD2		80
#define SCANCODE_CURSORDOWN		80
#define SCANCODE_KEYPAD3		81
#define SCANCODE_CURSORDOWNRIGHT	81
#define SCANCODE_KEYPAD0		82
#define SCANCODE_KEYPADPERIOD		83

#define SCANCODE_LESS			86

#define SCANCODE_F11			87
#define SCANCODE_F12			88

#define SCANCODE_KEYPADENTER		0x11c
#define SCANCODE_RIGHTCONTROL		0x11d
#define SCANCODE_KEYPADDIVIDE		0x135
#define SCANCODE_PRINTSCREEN		0x137
#define SCANCODE_RIGHTALT		0x138
#define SCANCODE_NUMLOCK		0x145
#define SCANCODE_BREAK			0x146

#define SCANCODE_HOME			0x147
#define SCANCODE_CURSORBLOCKUP		0x148	/* Cursor key block */
#define SCANCODE_PAGEUP			0x149
#define SCANCODE_CURSORBLOCKLEFT	0x14b	/* Cursor key block */
#define SCANCODE_CURSORBLOCKRIGHT	0x14d	/* Cursor key block */
#define SCANCODE_END			0x14f
#define SCANCODE_CURSORBLOCKDOWN	0x150	/* Cursor key block */
#define SCANCODE_PAGEDOWN		0x151
#define SCANCODE_INSERT			0x152
#define SCANCODE_REMOVE			0x153

char *keynames[512] = {
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
  "Pause",             /*  69 */
//  "Num_Lock",          /*  69 */
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
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown"
};

int scancode[8] = { 0, 0, 0, 0,
                    0, 0, 0, 0 };

extern HANDLE hWnd;
extern int nodemo;

ui8 keybits=0;
int lastkeycode=0;
int lastkey=0;
int buckybits=0;

int in_focus=0;
int single=0;
int multiple=0;

static int lcbit  = 1<<0;
static int labit  = 1<<1;
static int lsbit  = 1<<2;
static int rcbit  = 1<<3;
static int rabit  = 1<<4;
static int rsbit  = 1<<5;
/*
static int clbit  = 1<<6;
static int clsbit = 1<<7;
static int slbit  = 1<<8;
static int slsbit = 1<<9;
static int nlbit  = 1<<10;
static int nlsbit = 1<<11;
*/

extern void storescr(void);
extern void paintscr(void);
void keypump(void);
void keyevent(int key, int keycode);
LRESULT __stdcall WindowProc(HWND, UINT, WPARAM, LPARAM);

void
keyevent(int key, int keycode)
{
  int up;

  up = keycode&0x80000000;
  lastkeycode = keycode;
  keycode&=0x7fffffff;

  switch(keycode) {
  case SCANCODE_LEFTCONTROL:
    buckybits = (buckybits&~lcbit) | (up?0:lcbit);
    break;
  case SCANCODE_LEFTALT:
    buckybits = (buckybits&~labit) | (up?0:labit);
    break;
  case SCANCODE_LEFTSHIFT:
    buckybits = (buckybits&~lsbit) | (up?0:lsbit);
    break;
  case SCANCODE_RIGHTCONTROL:
    buckybits = (buckybits&~rcbit) | (up?0:rcbit);
    break;
  case SCANCODE_RIGHTALT:
    buckybits = (buckybits&~rabit) | (up?0:rabit);
    break;
  case SCANCODE_RIGHTSHIFT:
    buckybits = (buckybits&~rsbit) | (up?0:rsbit);
    break;
  case SCANCODE_CAPSLOCK:
  case SCANCODE_SCROLLLOCK:
  case SCANCODE_NUMLOCK:
    break;
  default:
    lastkey = up | ((buckybits & (lsbit|rsbit)) ? key : (isalpha(key) ? (islower(key) ? key : tolower(key)) : key));
    break;
  }

  if(single)
    return;

  // multiple

  if(keycode == scancode[0])
    keybits = (keybits & ~left_bit)
    | (up?0:left_bit);
  if(keycode == scancode[1])
    keybits = (keybits & ~right_bit)
    | (up?0:right_bit);
  if(keycode == scancode[2])
    keybits = (keybits & ~thrust_bit)
    | (up?0:thrust_bit);
  if(keycode == scancode[3])
    keybits = (keybits & ~fire_bit)
    | (up?0:fire_bit);
  if(keycode == scancode[4])
    keybits = (keybits & ~pickup_bit)
    | (up?0:pickup_bit);
  if(keycode == scancode[5])
    keybits = (keybits & ~pause_bit)
    | (up?0:pause_bit);
  if(keycode == scancode[6]
     || keycode == scancode[7])
    keybits = (keybits & ~escape_bit)
    | (up?0:escape_bit);
}

#include <stdio.h>

LRESULT __stdcall
WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
//  LRESULT lres;
  static int focus = 3;

  switch(Msg) {
  case WM_KEYDOWN:
  case WM_KEYUP:
    keyevent(wParam, ((lParam>>16)&0x1ff) | (lParam&0x80000000));
    break;

  case WM_CLOSE:
    raise(SIGINT);
//    PostThreadMessage(GetCurrentThreadId(), WM_QUIT, 1, 0);
    break;
  case WM_MOVE:
    {
      RECT rect;
      GetWindowRect(hWnd, &rect);
      MoveWindow(hWnd, 0, 0, rect.right-rect.left, rect.bottom-rect.top, TRUE);
    }
    break;
  case WM_NCACTIVATE:
  case 6: /* WM_ACTIVATE: */
    if(!wParam) {
      if(focus == 0)
        break;
      focus = 0;
//      if(multiple)
//        keyevent('P', SCANCODE_P);
      storescr();
    }
    break;
  case 28: /* WM_ACTIVATEAPP: */
    break;
  case WM_CANCELMODE:
    if(focus == 0)
      break;
    focus = 0;
//    if(multiple)
//      keyevent('P', SCANCODE_P);
    storescr();
    break;
  case WM_SETFOCUS:
    if(focus == 0)
      focus = 1;
    break;
  case WM_PAINT:
    if(focus != 1)
      break;
    focus = 2;
    paintscr();
    break;
  case WM_NCPAINT:
  case WM_ERASEBKGND:
    fflush(stdout);
    return 0;
  case WM_SYSCOMMAND:
    if(wParam == SC_CLOSE)
      return 0;
    if(wParam == SC_MOVE)
      return 0;
    if(wParam == SC_KEYMENU)
      return 0;
    break;
  }

  return DefWindowProc(hWnd, Msg, wParam, lParam);
}

void
keypump(void)
{
  MSG msg;
  {//if(single) {
    while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
      if(!GetMessage(&msg, NULL, 0, 0))
	return;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

void
singlekey(void)
{
  single = 1;
  lastkey = 0;
  multiple = 0;

  lastkey = 0;
}

void
multiplekeys(void)
{
  single = 0;
  multiple = 1;
  lastkeycode = 0;

  keybits = 0;
}

int
getonemultiplekey(void)
{
  MSG msg;
  int state=0;
  int gotkey=0;

  do {
    if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
      if(!GetMessage(&msg, NULL, 0, 0))
	return 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if(!state) {
	if(lastkeycode && (~(lastkeycode&0x80000000)))
	  state = lastkeycode;
	continue;
      }
      if((state|0x80000000) == (unsigned int)lastkeycode)
	gotkey = state;
    }
  } while(gotkey==0);

  return gotkey;
}

int
getkey(void)
{
  MSG msg;
  static int state=1;
  static int mem=0xffffffff;

  if(mem == 0xffffffff) {
    state = 1;
    mem = lastkey;
  }

  while(1) {
    if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
      if(!GetMessage(&msg, NULL, 0, 0))
	return 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if(state && (mem==lastkey))
	continue;
      state = 0;
      mem = lastkey;
      if(lastkey&0x80000000) {
	mem = 0xffffffff;
	return lastkey&0x7fffffff;
      }
    }
    else
      return 0;
  }
  return 0;
}

ui8
getkeys(void)
{
  MSG msg;
  if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
    if(!GetMessage(&msg, NULL, 0, 0))
      return keybits;
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    return keybits;
  }

  return keybits;
}

char *
keystring(int key)
{
  static char keybuffer[100];
  unsigned int i;

  if(key<0 || key>=512)
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
  unsigned int i;

  strncpy(keybuffer, keyname, 99);
  keybuffer[99] = '\0';
  for(i=0; i<strlen(keybuffer); i++)
    if(keybuffer[i] == ' ')
      keybuffer[i] = '_';

  for(i=0; i<512; i++) {
    if(!keynames[i])
      continue;
    if(!strcasecmp(keynames[i], keybuffer))
      return(i);
  }

  return(0);
}

void
flushkeyboard(void)
{
  keybits = 0;
}

int
keywaiting(void)
{
  MSG msg;
  if(PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE))
    return TRUE;

  if(!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    return FALSE;

  if(msg.message == WM_KEYDOWN)
    return TRUE;
  if(!GetMessage(&msg, NULL, 0, 0))
    return FALSE;
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  return FALSE;
}

int
keyinit(void)
{
  int i;

  for(i=0; i<8; i++) {
    if(scancode[i] != 0)
      i=9;
  }

  if(i==8) {
    scancode[0] = SCANCODE_A;
    scancode[1] = SCANCODE_S;
    scancode[2] = SCANCODE_RIGHTCONTROL;
    scancode[3] = SCANCODE_ENTER;
    scancode[4] = SCANCODE_SPACE;
    scancode[5] = SCANCODE_P;
    scancode[6] = SCANCODE_ESCAPE;
    scancode[7] = SCANCODE_Q;
  }

  for(i=128; i<512; i++)
    keynames[i] = "Unknown";

  keynames[SCANCODE_KEYPADENTER] = "Keypad_Enter";
  keynames[SCANCODE_RIGHTCONTROL] = "Right_Control";
  keynames[SCANCODE_KEYPADDIVIDE] = "Keypad_Divide";
  keynames[SCANCODE_PRINTSCREEN] = "Print_Screen";
  keynames[SCANCODE_RIGHTALT] = "Right_Alt";
  keynames[SCANCODE_NUMLOCK] = "Num_Lock";
  keynames[SCANCODE_BREAK] = "Break";
  keynames[SCANCODE_HOME] = "Home";
  keynames[SCANCODE_CURSORBLOCKUP] = "Cursor_Up";
  keynames[SCANCODE_PAGEUP] = "Page_Up";
  keynames[SCANCODE_CURSORBLOCKLEFT] = "Cursor_Left";
  keynames[SCANCODE_CURSORBLOCKRIGHT] = "Cursor_Right";
  keynames[SCANCODE_END] = "End";
  keynames[SCANCODE_CURSORBLOCKDOWN] = "Cursor_Down";
  keynames[SCANCODE_PAGEDOWN] = "Page_Down";
  keynames[SCANCODE_INSERT] = "Insert";
  keynames[SCANCODE_REMOVE] = "Delete";

  multiplekeys();
  return 0;
}

int
keyclose(void)
{
  return 0;
}

char *
keyname(void)
{
  static char name[] = "Win32";

  return name;
}
