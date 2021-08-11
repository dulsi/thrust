
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

ui8 palette_shift=0;
ui8 color_conversion[USED_COLORS] = {
  0x00, 0x01, 0x02,       0x04, 
                          0x0c,       0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19,       0x1b, 0x1c, 0x1d, 0x1e, 
  0x20, 0x21, 0x22, 0x23, 0x24,



        0x41,







  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89,                               0x8f,


                          0xa4, 0xa5,



  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
  0xc8,





                                      0xfe, 0xff
};
ui8 color_lookup[256];

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
      if(*storage>res && (!shield || (shield && *storage<SHIELDLIMIT)))
	res=*storage;
    }
    storage++;
  }
  return CRASH_VALUE(res);
}
