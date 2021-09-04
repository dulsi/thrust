
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>

#include "thrust_t.h"
#include "thrust.h"
#include "fast_gr.h"
#include "font5x5.h"
#include "gr_drv.h"

ui8 *bild;

static ui8 fuelblink;

ui8 pixel_collision[256];

ui8 extracolor[EXTRA_COLORS * 3] = {
 0x40, 0x40, 0xff,
 0xff, 0xff, 0xff,
 0xd5, 0x00, 0x00,
 0xd5, 0xaa, 0x00,
 0x00, 0xd5, 0x00,
 0xff, 0xff, 0x49,
 0xff, 0xff, 0x49,
 0xff, 0xff, 0x49,
 0xa8, 0xa8, 0xff,
 0x00, 0x55, 0x00,
 0xde, 0xf7, 0x08,
 0x6b, 0x00, 0x00,
};
ui8 extracolor_shift = 0;

ui8 findcolor[FIND_COLORS * 3] = {
 0xff, 0x80, 0x35,
 0xa8, 0xa8, 0xff,
 0xff, 0x55, 0xaa,
 0x80, 0xff, 0xaa,
 0xaa, 0xff, 0xaa
};
ui8 foundcolor[FIND_COLORS];

ui8 bullet_shift = 0;
ui8 shield_shift = 0;
ui8 ship_shift = 0;

void
putscr(int x, int y)
{
  if(PUSEY<=PBILDY-y)
    putarea(bild, x, y, PUSEX, PUSEY, PBILDX<<1, 0, 24);
  else {
    putarea(bild, x, y, PUSEX, PBILDY-y, PBILDX<<1, 0, 24);
    putarea(bild, x, 0, PUSEX, PUSEY-PBILDY+y, PBILDX<<1, 0, PBILDY+24-y);
  }
}

void
putblock(int x, int y, ui8 *source)
{
  int i;
  ui8 *dest1, *dest2;

  dest1=bild+((y<<3)*(PBILDX<<1))+(x<<3);
  dest2=dest1+((x>=BBILDX)?-(PBILDX):(PBILDX));
  
  for(i=0; i<8; i++) {
    memcpy(dest1, source, 8);
    memcpy(dest2, source, 8);
    source+=8;
    dest1+=PBILDX<<1;
    dest2+=PBILDX<<1;
  }
}

void
drawfuel(int fuel)
{
  char str[16];
  ui8 tmpcol, tmppap, tmpflg;

  tmpcol=chcolor;
  tmppap=chpaper;
  tmpflg=chflag;
  chpaper=BLACK;
  fuelblink=(fuelblink-1)&31;
  if(((fuel+39)/40)>40 || fuelblink&16)
    chcolor=FUELHIGHLIGHT;
  else
    chcolor=FUELCOLOR;
  chflag=1;
  sprintf(str, "  %d", (fuel+3)/4);
  printgs(80-gstrlen(str), 12, str);
  chflag=tmpflg;
  chpaper=tmppap;
  chcolor=tmpcol;
}

void
drawship(word bx, word by, ui8 *ship, ui8 *storage, int size)
{
  ui8 *maxtmp, *tmp, pix;
  int i,j;

  maxtmp=bild+((PBILDY-1)*PBILDX<<1);
  tmp=bild+by*(PBILDX<<1)+bx;
  for(i=0; i<size*size; i+=size) {
    memcpy(storage+i, tmp, size);
    for(j=0; j<size; j++) {
      pix=*(ship++);
      if(pix!=BLACK)
	*(tmp+j)=pix;
    }
    if(tmp>=maxtmp)
      tmp=bild+bx;
    else
      tmp+=PBILDX<<1;
  }
}

void
undrawship(word bx, word by, ui8 *storage, int size)
{
  ui8 *maxtmp, *tmp;
  int i;

  maxtmp=bild+((PBILDY-1)*PBILDX<<1);
  tmp=bild+by*(PBILDX<<1)+bx;
  for(i=0; i<size*size; i+=size) {
    memcpy(tmp, storage+i, size);
    if(tmp>=maxtmp)
      tmp=bild+bx;
    else
      tmp+=PBILDX<<1;
  }    
}

void
drawsquare(word bx, word by,
	   ui8 *object, ui8 *storage,
	   ui8 deltax, ui8 deltay)
{
  ui8 *maxtmp, *tmp, pix;
  word i,j;
  word deltaxy;

  deltaxy=(word)deltax*deltay;
  maxtmp=bild+((PBILDY-1)*PBILDX<<1);
  tmp=bild+by*(PBILDX<<1)+bx;
  for(i=0; i<deltaxy; i+=(int)deltax) {
    memcpy(storage+i, tmp, (int)deltax);
    for(j=0; j<(int)deltax; j++) {
      pix=*(object++);
      if(pix!=BLACK)
	*(tmp+j)=pix;
    }
    if(tmp>=maxtmp)
      tmp=bild+bx;
    else
      tmp+=PBILDX<<1;
  }
}

void
undrawsquare(word bx, word by,
	     ui8 *storage,
	     ui8 deltax, ui8 deltay)
{
  ui8 *maxtmp, *tmp;
  word i;
  word deltaxy;

  deltaxy=(word)deltax*deltay;
  maxtmp=bild+((PBILDY-1)*PBILDX<<1);
  tmp=bild+by*(PBILDX<<1)+bx;
  for(i=0; i<deltaxy; i+=(int)deltax) {
    memcpy(tmp, storage+i, (int)deltax);
    if(tmp>=maxtmp)
      tmp=bild+bx;
    else
      tmp+=PBILDX<<1;
  }
}

word
testcrash(ui8 *object, ui8 *storage, word len, ui8 shield)
{
  word i;
  ui8 res=0;

  for(i=0; i<len; i++) {
    if(*(object++)!=BLACK) {
      if(pixel_collision[*storage]>res && (!shield || (shield && pixel_collision[*storage]>SHIELDLIMIT))) {
        res=pixel_collision[*storage];
        printf("Found %d\n", *storage);
      }
    }
    storage++;
  }
  return CRASH_VALUE(res);
}
