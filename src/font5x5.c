
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "thrust_t.h"
#include "keyboard.h"
#include "thrust.h"
#include "font5x5.h"
#include "fast_gr.h"
#include "gr_drv.h"

ui8 chcolor=0;
ui8 chpaper=0;
ui8 chflag=0;

void
printgc(int x, int y, ui8 ch)
{
  int dx;
  int dy;
  char width;
  ui8 bits;
  ui8 bit;

  /*  ch = toupper(ch);*/
  width = font[ch][0];
  bit = bits = 1<<(width-1);
  for(dy=1; dy<6; dy++, bit=bits) {
    for(dx=0; dx<width; dx++, bit>>=1) {
      if(font[ch][dy] & bit)
	putpixel(x+dx, y+dy-1, chcolor);
      else if(chflag)
	putpixel(x+dx, y+dy-1, chpaper);
    }
    if(chflag)
      putpixel(x+dx, y+dy-1, chpaper);
  }
}

int
printgs(int x, int y, char *string)
{
  ui8 i;
  ui8 len=strlen(string);
  ui8 ch;
  int ox=x;

  for(i=0; i<len; i++) {
    /*    ch=toupper(*(string+i));*/
    ch=*(string+i);
    if(ch < 128)
      switch(ch)
	{
	case 13:
	  x=ox;
	  break;
	case 10:
	  y+=6;
	  x=ox;
	  break;
	default:
	  printgc(x, y, ch);
	  x += font[ch][0]+1;
	}
  }

  return(x);
}

int
readgs(int x, int y, char string[], int maxc, int maxp, char flag)
{
  int i, j;
  int key;
  ui8 ch;
  ui8 temp=chcolor;
  int leftx=x;
  ui8 cursor=0;
  int xc;

  maxp+=x;
  i=strlen(string);
  x=printgs(x, y, string);

  do {
    do {
      key=0;
      cursor=1-cursor;
      for(j=0; j<12 && !key; j++) {
	key=getkey();
	if(key >= 128)
	  key = 0;
	if(!key)
	  usleep(50000L);
      }
      /*      vga_lockvc();*/
      for(j=0; j<5; j++)
	putpixel(x+j, y+5, (ui8)((int)cursor?temp:chpaper));
      displayscreen(0UL);
      /*      vga_unlockvc();*/
    } while(!key);
    /*    vga_lockvc();*/
    for(j=0; j<5; j++)
      putpixel(x+j, y+5, chpaper);
    displayscreen(0UL);
    /*    vga_unlockvc();*/

    /*ch=toupper(key);*/
    ch=key;
    switch(ch) {
    case 10:
    case 13:
      string[i]=0;
      ch=27;
      break;
    case 27:
      x=-1;
      break;
    case 127:
    case 8:
      if(i) {
	xc = 1+font[(int)string[--i]][0];
	x -= xc;
	chcolor=chpaper;
	/*	vga_lockvc();*/
	printgc(x, y, string[i]);
	displayscreen(0UL);
	/*	vga_unlockvc();*/
	chcolor=temp;
      }
      break;
    default:
      if(!font[ch][0]) {
	printf("\007");
	fflush(stdout);
      }
      else if(i!=maxc && x+(xc=1+font[ch][0])<=maxp) {
	string[i++]=ch;
	/*	vga_lockvc();*/
	printgc(x, y, ch);
	displayscreen(0UL);
	/*	vga_unlockvc();*/
	x+=xc;
      }
    }
  }
  while(ch!=27);

  if(flag) {
    /*    vga_lockvc();*/
    for(i=y; i<y+5; i++)
      for(xc=leftx; xc<maxp; xc++)
	putpixel(xc, i, chpaper);
    /*    vga_unlockvc();*/
    displayscreen(0UL);
  }
  return(x);
}

int
gstrlen(char *string)
{
  int len=strlen(string);
  int width=0;
  int count=0;
  int i;
  ui8 ch;

  for(i=0; i<len; i++) {
    /*ch = toupper(*(string+i));*/
    ch = *(string+i);
    if(ch < 128)
      switch(ch) {
      case 13:
      case 10:
	if(count>width)
	  width=count;
	count=0;
	break;
      default:
	count += font[ch][0]+1;
      }
  }

  return((count>width) ? count : width);
}
