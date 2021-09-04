
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "thrust_t.h"
#include "graphics.h"
#include "things.h"
#include "fast_gr.h"
#include "gr_drv.h"
#include "thrust.h"

static int colors_inverted = 0;


void
writeblock(word bx, word by, ui8 block)
{
  word tempx, tempy;

  *(bana+bx+by*lenx)=block;
  tempx=bx;
  tempy=by;
  if(pblockx+BBILDX>(int)lenx && tempx<BBILDX)
    tempx+=lenx;
  if(pblocky+BBILDY>(int)leny && tempy<BBILDY)
    tempy+=leny;
  if(insideblock(tempx, tempy, pblockx, pblocky, 0, 0))
    putblock(bblockx-pblockx+tempx, tempy%BBILDY,
	     blocks+(block<<6));
}

#define NR_TP (6)
#define SZ_TP (3)
#define DIFF_TP (8)
#define SHIFT_TP (10)

void
drawteleline(int round, int x1, int y1, int x2, int y2, int j, int k)
{
  int l;
  static ui8 telemem[2*7*4*NR_TP*SZ_TP];
  static ui8 *tm;

  switch(round) {
  case 0:
    tm=&telemem[0];
    break;
  case 1:
    for(l=-3; l<=3; l++) {
      putpixel(x1+(j+2)*DIFF_TP+k-SHIFT_TP, 24+y1+l, SHIP);
      putpixel(x1-(j+2)*DIFF_TP-k+SHIFT_TP, 24+y1+l, SHIP);
      putpixel(x1+l, 24+y1+(j+2)*DIFF_TP+k-SHIFT_TP, SHIP);
      putpixel(x1+l, 24+y1-(j+2)*DIFF_TP-k+SHIFT_TP, SHIP);
      if(loaded) {
	putpixel(x2+(j+2)*DIFF_TP+k-SHIFT_TP, 24+y2+l, POD);
	putpixel(x2-(j+2)*DIFF_TP-k+SHIFT_TP, 24+y2+l, POD);
	putpixel(x2+l, 24+y2+(j+2)*DIFF_TP+k-SHIFT_TP, POD);
	putpixel(x2+l, 24+y2-(j+2)*DIFF_TP-k+SHIFT_TP, POD);
      }
    }
    break;
  case 2:
    for(l=-3; l<=3; l++) {
      *(tm++) = *(bild + (bildx+x1+(j+2)*DIFF_TP+k-SHIFT_TP)
		  + ((bildy+y1+l)%PBILDY)*PBILDX*2);
      *(tm++) = *(bild + (bildx+x1-(j+2)*DIFF_TP-k+SHIFT_TP)
		  + ((bildy+y1+l)%PBILDY)*PBILDX*2);
      *(tm++) = *(bild + (bildx+x1+l)
		  + ((bildy+y1+(j+2)*DIFF_TP+k-SHIFT_TP)%PBILDY)*PBILDX*2);
      *(tm++) = *(bild + (bildx+x1+l)
		  + ((bildy+y1-(j+2)*DIFF_TP-k+SHIFT_TP)%PBILDY)*PBILDX*2);
      if(loaded) {
	*(tm++) = *(bild + (bildx+x2+(j+2)*DIFF_TP+k-SHIFT_TP)
		    + ((bildy+y2+l)%PBILDY)*PBILDX*2);
	*(tm++) = *(bild + (bildx+x2-(j+2)*DIFF_TP-k+SHIFT_TP)
		    + ((bildy+y2+l)%PBILDY)*PBILDX*2);
	*(tm++) = *(bild + (bildx+x2+l)
		    + ((bildy+y2+(j+2)*DIFF_TP+k-SHIFT_TP)%PBILDY)*PBILDX*2);
	*(tm++) = *(bild + (bildx+x2+l)
		    + ((bildy+y2-(j+2)*DIFF_TP-k+SHIFT_TP)%PBILDY)*PBILDX*2);
      }
    }
    break;
  case 3:
    for(l=-3; l<=3; l++) {
      putpixel(x1+(j+2)*DIFF_TP+k-SHIFT_TP, 24+y1+l, *(tm++));
      putpixel(x1-(j+2)*DIFF_TP-k+SHIFT_TP, 24+y1+l, *(tm++));
      putpixel(x1+l, 24+y1+(j+2)*DIFF_TP+k-SHIFT_TP, *(tm++));
      putpixel(x1+l, 24+y1-(j+2)*DIFF_TP-k+SHIFT_TP, *(tm++));
      if(loaded) {
	putpixel(x2+(j+2)*DIFF_TP+k-SHIFT_TP, 24+y2+l, *(tm++));
	putpixel(x2-(j+2)*DIFF_TP-k+SHIFT_TP, 24+y2+l, *(tm++));
	putpixel(x2+l, 24+y2+(j+2)*DIFF_TP+k-SHIFT_TP, *(tm++));
	putpixel(x2+l, 24+y2-(j+2)*DIFF_TP-k+SHIFT_TP, *(tm++));
      }
    }
    break;
  }
}

void
drawteleport(int tohere)
{
  int i, j, k;
  int x1, y1, x2, y2;

  x1=x2=154+7+shipdx;
  y1=y2= 82+7+shipdy;
  if(loaded) {
    x2=161-(int)((LOADLINELEN-loadpoint)*cos(alpha)/7.875);
    y2= 89+(int)((LOADLINELEN-loadpoint)*sin(alpha)/7.875);
  }

  syncscreen(0UL);
  for(i=0; i<NR_TP+SZ_TP-1; i++) {
    for(k=min(SZ_TP-1, i), j=max(i-(SZ_TP-1), 0); j<=min(i, NR_TP-1); k--, j++)
      drawteleline(1, x1, y1, x2, y2, j, k);
    displayscreen(30000UL);
  }

  syncscreen(250000UL);
  if(tohere)
    drawshuttle();
  putscr(bildx, bildy);

  for(i=0; i<NR_TP+SZ_TP-1; i++)
    for(k=min(SZ_TP-1, i), j=max(i-(SZ_TP-1), 0); j<=min(i, NR_TP-1); k--, j++)
      drawteleline(1, x1, y1, x2, y2, j, k);
  displayscreen(0UL);
  drawteleline(0, 0, 0, 0, 0, 0, 0);
  for(i=0; i<NR_TP+SZ_TP-1; i++)
    for(k=min(SZ_TP-1, i), j=max(i-(SZ_TP-1), 0); j<=min(i, NR_TP-1); k--, j++)
      drawteleline(2, x1, y1, x2, y2, j, k);

  syncscreen(250000UL);
  drawteleline(0, 0, 0, 0, 0, 0, 0);
  for(i=0; i<NR_TP+SZ_TP-1; i++) {
    for(k=min(SZ_TP-1, i), j=max(i-(SZ_TP-1), 0); j<=min(i, NR_TP-1); k--, j++)
      drawteleline(3, x1, y1, x2, y2, j, k);
    displayscreen(30000UL);
  }
  if(tohere)
    undrawshuttle();
}

void
swap(int *pa, int *pb)
{
  int	t;
  t=*pa; *pa=*pb; *pb=t;
}

void
drawlinev(int x1, int y1, int x2, int y2, ui8 color, ui8 *storage)
{
  int d, dx, dy;
  int Ai, Bi, xi;
  ui8 *ptr=bild;

  if(y1>y2) {
    swap(&x1, &x2);
    swap(&y1, &y2);
  }
  xi=(x2>x1)?1:-1;
  dx=abs(x2-x1);
  dy=(y2-y1);
  Ai=(dx-dy)<<1;
  Bi=(dx<<1);
  d=Bi-dy;

  ptr+=y1*(PBILDX<<1);
  *(storage++)=*(ptr+x1);
  *(ptr+x1)=color;
  for(y1++, ptr+=PBILDX<<1; y1<=y2; y1++, ptr+=PBILDX<<1) {
    if(y1==PBILDY)
      ptr=bild;
    if(d<0)
      d+=Bi;
    else {
      x1+=xi;
      d+=Ai;
    }
    *(storage++)=*(ptr+x1);
    *(ptr+x1)=color;
  }
}

void
undrawlinev(int x1, int y1, int x2, int y2, ui8 *storage)
{
  int d, dx, dy;
  int Ai, Bi, xi;
  ui8 *ptr=bild;

  if(y1>y2) {
    swap(&x1, &x2);
    swap(&y1, &y2);
  }
  xi=(x2>x1)?1:-1;
  dx=abs(x2-x1);
  dy=(y2-y1);
  Ai=(dx-dy)<<1;
  Bi=(dx<<1);
  d=Bi-dy;

  ptr+=y1*(PBILDX<<1);
  *(ptr+x1)=*(storage++);
  for(y1++, ptr+=PBILDX<<1; y1<=y2; y1++, ptr+=PBILDX<<1) {
    if(y1==PBILDY)
      ptr=bild;
    if(d<0)
      d+=Bi;
    else {
      x1+=xi;
      d+=Ai;
    }
    *(ptr+x1)=*(storage++);
  }
}

void
drawlineh(int x1, int y1, int x2, int y2, ui8 color, ui8 *storage)
{
  int d, dx, dy;
  int Ai, Bi, yi, i;
  ui8 *ptr=bild;

  if(x1>x2) {
    swap(&x1, &x2);
    swap(&y1, &y2);
  }
  if(y2>y1) {
    yi=PBILDX<<1;
    i=1;
  }
  else {
    yi=-PBILDX<<1;
    i=-1;
  }
  dx=x2-x1;
  dy=abs(y2-y1);
  Ai=(dy-dx)<<1;
  Bi=(dy<<1);
  d=Bi-dx;
  if(y1>=PBILDY)
    y1-=PBILDY;
  y2=y1;
  y1=y1*PBILDX<<1;

  ptr+=y1;
  *(storage++)=*(ptr+x1);
  *(ptr+x1)=color;
  for(x1++; x1<=x2; x1++) {
    if(d<0)
      d+=Bi;
    else {
      ptr+=yi;
      y2+=i;
      if(y2==-1)
	ptr+=PBILDY*PBILDX<<1;
      if(y2==PBILDY)
	ptr=bild;
      d+=Ai;
    }
    *(storage++)=*(ptr+x1);
    *(ptr+x1)=color;
  }
}

void
undrawlineh(int x1, int y1, int x2, int y2, ui8 *storage)
{
  int d, dx, dy;
  int Ai, Bi, yi, i;
  ui8 *ptr=bild;

  if(x1>x2) {
    swap(&x1, &x2);
    swap(&y1, &y2);
  }
  if(y2>y1) {
    yi=PBILDX<<1;
    i=1;
  }
  else {
    yi=-PBILDX<<1;
    i=-1;
  }
  dx=x2-x1;
  dy=abs(y2-y1);
  Ai=(dy-dx)<<1;
  Bi=(dy<<1);
  d=Bi-dx;
  if(y1>=PBILDY)
    y1-=PBILDY;
  y2=y1;
  y1=y1*PBILDX<<1;

  ptr+=y1;
  *(ptr+x1)=*(storage++);
  for(x1++; x1<=x2; x1++) {
    if(d<0)
      d+=Bi;
    else {
      ptr+=yi;
      y2+=i;
      if(y2==-1)
        ptr+=PBILDY*PBILDX<<1;
      if(y2==PBILDY)
        ptr=bild;
      d+=Ai;
    }
    *(ptr+x1)=*(storage++);
  }
}

void
drawline(int x1, int y1, int x2, int y2, ui8 color, ui8 *storage)
{
  if(y1>y2+64)
    y2+=PBILDY;
  if(y2>y1+64)
    y1+=PBILDY;
  if(abs(x1-x2)<abs(y1-y2))
    drawlinev(x1, y1, x2, y2, color, storage);
  else
    drawlineh(x1, y1, x2, y2, color, storage);
}

void
undrawline(int x1, int y1, int x2, int y2, ui8 *storage)
{
  if(y1>y2+64)
    y2+=PBILDY;
  if(y2>y1+64)
    y1+=PBILDY;
  if(abs(x1-x2)<abs(y1-y2))
    undrawlinev(x1, y1, x2, y2, storage);
  else
    undrawlineh(x1, y1, x2, y2, storage);
}

void
drawbullets(void)
{
  int l;
  bullet *bulletptr;
  word tempx, tempy;
  ui8 target;

  for(l=0, bulletptr=bullets; l<maxbullets; l++, bulletptr++)
    if((*bulletptr).life) {
      tempx=(*bulletptr).x>>3;
      tempy=(*bulletptr).y>>3;
      if(pixx+PUSEX>(int)lenx3 && tempx<PUSEX)
        tempx+=lenx3;
      if(pixy+PUSEY>(int)leny3 && tempy<PUSEY)
        tempy+=leny3;
      if(insidepixel(tempx, tempy, pixx, pixy, 4, 4))
        drawsquare(bildx+tempx-pixx, tempy%PBILDY,
          bulletmap+((*bulletptr).dir<<4),
          bulletstorage+(l<<4), 4, 4);
      else {
        target = *(bana+(tempx>>3)%lenx+((tempy>>3)%leny)*lenx);
        if(target!=' ') {
          /* Add code to take care of offscreen hits */
          if((*bulletptr).owner)
            switch(target) {
            case '`':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case ']':
            case '\\':
            case '^':
            case '_':
              hit((tempx+3)%lenx3, (tempy+3)%leny3, 4, (*bulletptr).owner);
            }
          (*bulletptr).life=0;
        }
      }
    }
}

void
undrawbullets(void)
{
  int l;
  bullet *bulletptr;
  word tempx, tempy;
  word crash;

  for(l=maxbullets-1, bulletptr=bullets+maxbullets-1; l>=0; l--, bulletptr--)
    if((*bulletptr).life) {
      tempx=(*bulletptr).x>>3;
      tempy=(*bulletptr).y>>3;
      if(pixx+PUSEX>(int)lenx3 && tempx<PUSEX)
        tempx+=lenx3;
      if(pixy+PUSEY>(int)leny3 && tempy<PUSEY)
        tempy+=leny3;
      if(insidepixel(tempx, tempy, pixx, pixy, 4, 4)) {
        crash=testcrash(bulletmap+((*bulletptr).dir<<4),
          bulletstorage+(l<<4), 16, 0);
        if(crash) {
          if(crash>=3)
            hit((tempx+3)%lenx3, (tempy+3)%leny3, crash, (*bulletptr).owner);
          (*bulletptr).life=0;
        }
        undrawsquare(bildx+tempx-pixx, tempy%PBILDY,
          bulletstorage+(l<<4), 4, 4);
      }
    }
}

void
drawfragments(void)
{
  int l;
  fragment *fragmentptr;
  word tempx, tempy;
  static ui8 fragmentmap[4];
  static int fragmentinit=1;

  if(fragmentinit) {
    for(l=0; l<sizeof(fragmentmap); l++)
      fragmentmap[l] = FRAGMENT;
    fragmentinit=0;
  }

  for(l=0, fragmentptr=fragments; l<maxfragments; l++, fragmentptr++)
    if((*fragmentptr).life) {
      tempx=(*fragmentptr).x>>3;
      tempy=(*fragmentptr).y>>3;
      if(pixx+PUSEX>(int)lenx3 && tempx<PUSEX)
        tempx+=lenx3;
      if(pixy+PUSEY>(int)leny3 && tempy<PUSEY)
        tempy+=leny3;
      if(insidepixel(tempx, tempy, pixx, pixy, 2, 2))
        drawsquare(bildx+tempx-pixx, tempy%PBILDY,
          fragmentmap, fragmentstorage+(l<<2), 2, 2);
      else if(*(bana+(tempx>>3)%lenx+((tempy>>3)%leny)*lenx)!=' ')
        (*fragmentptr).life=0;
    }
}

void
undrawfragments(void)
{
  int l;
  fragment *fragmentptr;
  word tempx, tempy;
  word crash;
  static ui8 fragmentmap[4]={ 1, 1, 1, 1 };
  static int fragmentinit=1;

  if(fragmentinit) {
    for(l=0; l<sizeof(fragmentmap); l++)
      fragmentmap[l] = FRAGMENT;
    fragmentinit=0;
  }

  for(l=maxfragments-1, fragmentptr=fragments+maxfragments-1;
      l>=0;
      l--, fragmentptr--)
    if((*fragmentptr).life) {
      tempx=(*fragmentptr).x>>3;
      tempy=(*fragmentptr).y>>3;
      if(pixx+PUSEX>(int)lenx3 && tempx<PUSEX)
        tempx+=lenx3;
      if(pixy+PUSEY>(int)leny3 && tempy<PUSEY)
        tempy+=leny3;
      if(insidepixel(tempx, tempy, pixx, pixy, 2, 2)) {
        crash=testcrash(fragmentmap, fragmentstorage+(l<<2), 4, 0);
        if(crash) {
          (*fragmentptr).life=0;
        }
        undrawsquare(bildx+tempx-pixx, tempy%PBILDY,
               fragmentstorage+(l<<2), 2, 2);
      }
    }
}

void
drawpowerplantblip(void)
{
  word tempx, tempy;

  tempx=ppx;
  tempy=ppy;
  if(pblockx+BBILDX>(int)lenx && tempx<BBILDX)
    tempx+=lenx;
  if(pblocky+BBILDY>(int)leny && tempy<BBILDY)
    tempy+=leny;
  if(insideblock(tempx, tempy, pblockx, pblocky, 0, 0))
    putblock(bblockx-pblockx+tempx, tempy%BBILDY,
	     blocks+((ppblip?' ':222-(ppcount&0xc))<<6));
}

void
drawload(int flag)
{
  word tempx, tempy;

  tempx=loadbx;
  tempy=loadby;
  if(pblockx+BBILDX>(int)lenx && tempx<BBILDX)
    tempx+=lenx;
  if(pblocky+BBILDY>(int)leny && tempy<BBILDY)
    tempy+=leny;
  putblock(bblockx-pblockx+tempx, tempy%BBILDY,
	   blocks+((flag?'m':' ')<<6));

  tempx=loadbx+1;
  tempy=loadby;
  if(pblockx+BBILDX>(int)lenx && tempx<BBILDX)
    tempx+=lenx;
  if(pblocky+BBILDY>(int)leny && tempy<BBILDY)
    tempy+=leny;
  putblock(bblockx-pblockx+tempx, tempy%BBILDY,
	   blocks+((flag?'0':' ')<<6));

  tempx=loadbx;
  tempy=loadby+1;
  if(pblockx+BBILDX>(int)lenx && tempx<BBILDX)
    tempx+=lenx;
  if(pblocky+BBILDY>(int)leny && tempy<BBILDY)
    tempy+=leny;
  putblock(bblockx-pblockx+tempx, tempy%BBILDY,
	   blocks+((flag?'1':' ')<<6));

  tempx=loadbx+1;
  tempy=loadby+1;
  if(pblockx+BBILDX>(int)lenx && tempx<BBILDX)
    tempx+=lenx;
  if(pblocky+BBILDY>(int)leny && tempy<BBILDY)
    tempy+=leny;
  putblock(bblockx-pblockx+tempx, tempy%BBILDY,
	   blocks+((flag?'2':' ')<<6));

  tempx=loadbx;
  tempy=loadby+2;
  if(pblockx+BBILDX>(int)lenx && tempx<BBILDX)
    tempx+=lenx;
  if(pblocky+BBILDY>(int)leny && tempy<BBILDY)
    tempy+=leny;
  putblock(bblockx-pblockx+tempx, tempy%BBILDY,
	   blocks+((flag?'3':' ')<<6));

  tempx=loadbx+1;
  tempy=loadby+2;
  if(pblockx+BBILDX>(int)lenx && tempx<BBILDX)
    tempx+=lenx;
  if(pblocky+BBILDY>(int)leny && tempy<BBILDY)
    tempy+=leny;
  putblock(bblockx-pblockx+tempx, tempy%BBILDY,
	   blocks+((flag?'4':' ')<<6));
}

word
drawshuttle(void)
{
  word crash=0, tmp;
#ifdef DEBUG2
  int debug2i;
#endif
  int x1, x2=0, y1, y2=0, lx, ly;
  static ui8 wiremap[64];
  static int wireinit=1;
  
  if(wireinit) {
    for(tmp=0; tmp<sizeof(wiremap); tmp++)
      wiremap[tmp] = TRACTOR;
    wireinit=0;
  }

  if(loaded || loadcontact) {
    x1=bildx+161+shipdx;
    y1=(bildy+89+shipdy);
    if(loaded) {
      x2=bildx+161-(int)((LOADLINELEN-loadpoint)*cos(alpha)/7.875);
      y2=bildy+ 89+(int)((LOADLINELEN-loadpoint)*sin(alpha)/7.875);
    }
    else {
      x2=(loadbx<<3)+5;
      if(abs(x2-x1)>PBILDX/2)
        x2+=PBILDX;
      y2=(loadby<<3)+10;
    }
    lx=abs(x1-x2)%PBILDX;
    ly=abs(y1-y2)%PBILDY;
    if(lx>64)
      lx=abs(lx-PBILDX);
    if(ly>64)
      ly=abs(ly-PBILDY);
    drawline(x1, y1%PBILDY, x2, y2%PBILDY, TRACTOR, wirestorage);
    tmp=testcrash(wiremap, wirestorage, max(lx, ly)+1, shield);
    crash=max(crash, tmp);
#ifdef DEBUG2
    if(tmp) {
      printf("Crash: Wire destroyed. By %d. Wirelength %d.\n",
	     tmp, max(lx, ly)+1);
      printf("Wirestorage:");
      for(debug2i=0; debug2i<max(lx, ly)+1; debug2i++)
        printf(" %02x", *(wirestorage+debug2i));
      printf("\n");
      printf("Killer line: x1=%d, y1=%d, x2=%d, y2=%d\n",
	     x1, y1%PBILDY, x2, y2%PBILDY);
      undrawline(x1+2 , y1%PBILDY, x2+2 , y2%PBILDY, wirestorage);
    }
#endif
  }
  /* Draw the shuttle */
  if(shield>3)
    drawship(bildx+153+shipdx, (bildy+81+shipdy)%PBILDY,
	   shieldship+dir*17*17, shipstorage, 17);
  else
    drawship(bildx+154+shipdx, (bildy+82+shipdy)%PBILDY,
	   ship+(dir<<8), shipstorage, 16);
  tmp=testcrash(ship+(dir<<8), shipstorage, 256, shield);
  crash=max(crash, tmp);
#ifdef DEBUG2
  if(tmp)
    printf("Crash: Ship destroyed. By %d.\n", tmp);
#endif
  if(loaded || loadcontact) {
    if(loaded)
      drawsquare(x2-5, (y2-5)%PBILDY, loadmap, loadstorage, 11, 11);
    else if(loadcontact) {
      x1=loadbx<<3;
      y1=(loadby<<3)+5;
      if(pixx+PBILDX>(int)lenx3 && x1<PBILDX)
        x1+=lenx3;
      if(pixy+PBILDY>(int)leny3 && y1<PBILDY)
        y1+=leny3;
      drawsquare(bildx-pixx+x1, y1%PBILDY, loadmap, loadstorage, 11, 19);
    }
    tmp=testcrash(loadmap, loadstorage, 11*11, shield);
    crash=max(crash, tmp);
#ifdef DEBUG2
    if(tmp)
      printf("Crash: Load destroyed. By %d.\n", tmp);
#endif
  }
  return(crash);
}

void
undrawshuttle(void)
{
  int x1, x2=0, y1, y2=0;

  if(loaded) {
    x2=bildx+161-(int)((LOADLINELEN-loadpoint)*cos(alpha)/7.875);
    y2=bildy+ 89+(int)((LOADLINELEN-loadpoint)*sin(alpha)/7.875);
    undrawsquare(x2-5, (y2-5)%PBILDY, loadstorage, 11, 11);
  }
  else if(loadcontact) {
    x1=loadbx<<3;
    y1=(loadby<<3)+5;
    if(pixx+PBILDX>(int)lenx3 && x1<PBILDX)
      x1+=lenx3;
    if(pixy+PBILDY>(int)leny3 && y1<PBILDY)
      y1+=leny3;
    undrawsquare(bildx-pixx+x1, y1%PBILDY, loadstorage, 11, 19);
  }
  if(shield>3)
    undrawship(bildx+153+shipdx, (bildy+81+shipdy)%PBILDY,
	   shipstorage, 17);
  else
    undrawship(bildx+154+shipdx, (bildy+82+shipdy)%PBILDY,
	   shipstorage, 16);
  if(loaded || loadcontact) {
    x1=bildx+161+shipdx;
    y1=(bildy+89+shipdy);
    if(loadcontact) {
      x2=(loadbx<<3)+5;
      if(abs(x2-x1)>PBILDX/2)
	x2+=PBILDX;
      y2=(loadby<<3)+10;
    }
    undrawline(x1, y1%PBILDY, x2, y2%PBILDY, wirestorage);
  }
}

void
drawfuellines(void)
{
  if(shield>3)
    return;
  drawsquare(bildx+shipdx+151, (bildy+shipdy+98)%PBILDY,
	     fuelmap, fuelstorage, 4, 32);
  drawsquare(bildx+shipdx+168, (bildy+shipdy+98)%PBILDY,
	     fuelmap+128, fuelstorage+128, 4, 32);
}

void
undrawfuellines(void)
{
  if(shield>3)
    return;
  undrawsquare(bildx+shipdx+151, (bildy+shipdy+98)%PBILDY,
	       fuelstorage, 4, 32);
  undrawsquare(bildx+shipdx+168, (bildy+shipdy+98)%PBILDY,
	       fuelstorage+128, 4, 32);
}

void
setcolor(ui8 c, color *rgb)
{
  if(!rgb) {
    bin_colors[c*3+0] = 0;
    bin_colors[c*3+1] = 0;
    bin_colors[c*3+2] = 0;
  }
  else {
    bin_colors[c*3+0] = GAMMA(rgb->r);
    bin_colors[c*3+1] = GAMMA(rgb->g);
    bin_colors[c*3+2] = GAMMA(rgb->b);
  }

  if(!colors_inverted)
    return;

  bin_colors[c*3+0] = 255 - bin_colors[c*3+0];
  bin_colors[c*3+1] = 255 - bin_colors[c*3+1];
  bin_colors[c*3+2] = 255 - bin_colors[c*3+2];
}

void
invertedcolors(void)
{
  if(colors_inverted)
    return;

  colors_inverted = 1;
  normalcolors();
  colors_inverted = 1;
  fadepalette(0, 255, bin_colors, 64, 0);
}

void
normalcolors(void)
{
  int i;

  if(!colors_inverted)
    return;

  for(i=0; i<3*256; ++i)
    bin_colors[i] = 255 - bin_colors[i];

  colors_inverted = 0;
  fadepalette(0, 255, bin_colors, 64, 0);
}
