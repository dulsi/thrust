
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "thrust_t.h"
#include "level.h"
#include "things.h"
#include "fast_gr.h"
#include "graphics.h"
#include "thrust.h"

#ifdef __STDC__
int parsecolor(char *str, color *rgb);
#endif

int
matchsliders(void)
{
  word i,j;
  word match;
  int dist;

  for(i=0; i<nrsliders; i++) {
    if(!sliders[i].match && sliders[i].dir) {
      match=nrsliders;
      dist=lenx;
      for(j=0; j<nrsliders; j++) {
	if(!sliders[j].match && !sliders[j].dir
	   && ((sliders[i].type-1)/3==(sliders[j].type-1)/3)) {
	  switch(sliders[i].dir) {
	  case 1:
	    if(sliders[j].type!=12) { /* Horizontal sliders */
	      if((sliders[i].y1 == sliders[j].y1)
            && (sliders[j].x1>sliders[i].x1)
            && (sliders[j].x1-sliders[i].x1 < dist)) {
          dist=sliders[j].x1-sliders[i].x1;
          match=j;
	      }
	    }
	    else /* Vertical sliders */
	      if((sliders[i].x1 == sliders[j].x1)
            && (sliders[j].y1>sliders[i].y1)
            && (sliders[j].y1-sliders[i].y1 < dist)) {
          dist=sliders[j].y1-sliders[i].y1;
          match=j;
	      }	      
	    break;
	  case 2:
	    if(sliders[j].type!=12) { /* Horizontal sliders */
	      if((sliders[i].y1 == sliders[j].y1)
            && (sliders[i].x1>sliders[j].x1)
            && (sliders[i].x1-sliders[j].x1 < dist)) {
          dist=sliders[i].x1-sliders[j].x1;
          match=j;
	      }
	    }
	    else /* Vertical sliders */
	      if((sliders[i].x1 == sliders[j].x1)
            && (sliders[i].y1>sliders[j].y1)
            && (sliders[i].y1-sliders[j].y1 < dist)) {
          dist=sliders[i].y1-sliders[j].y1;
          match=j;
	      }
	    break;
	  }
	}
      }
      if(match==nrsliders) {
        printf("Found slider with no blocker.\n");
        return(0);
      }
      sliders[i].match=1;
      sliders[match].match=1;
      sliders[i].x2=sliders[match].x1;
      sliders[i].y2=sliders[match].y1;
      j=closestbutton((sliders[i].x1+sliders[i].x2)<<2,
		      (sliders[i].y1+sliders[i].y2)<<2);
      if(j<0) {
        printf("Found no botton to connect the slider with.\n");
        return(0);
      }
      j=majorbutton(j);
      sliders[i].next=((buttondata *)things[j].data)->sliders;
      ((buttondata *)things[j].data)->sliders=&sliders[i];
    }
  }

  return(1);
}

int
ismajorbutton(int tag)
{
  word i;

  for(i=0; i<nrthings; i++)
    if((things[i].type==7 || things[i].type==8)
      && (((buttondata *)things[i].data)->tag==tag))
	return(i);
  return(-1);
}

void
releasebana(void)
{
  word i;

  for(i=0; i<nrthings; i++)
    if(things[i].data)
      free(things[i].data);
  nrthings=0;
  nrsliders=0;
  nrbarriers=0;
  nrrestartpoints=0;
}

int
parsecolor(char *str, color *rgb)
{
  int i=0;

  for(; str[i] && str[i]==' '; i++);
  if(!str[i])
    return 0;
  rgb->r=atoi(&str[i]);
  for(; str[i] && str[i]!=' '; i++);
  for(; str[i] && str[i]==' '; i++);
  if(!str[i])
    return 0;
  rgb->g=atoi(&str[i]);
  for(; str[i] && str[i]!=' '; i++);
  for(; str[i] && str[i]==' '; i++);
  if(!str[i])
    return 0;
  rgb->b=atoi(&str[i]);
  return 1;
}

int
readbana(char **ptr)
     /*
       Reads the level into the variable 'bana'.
       The file starts with eight values, i.e
       123
       69
       8
       10
       16
       R G B
       These eight numbers can be described as follows:
       The level is 123 blocks wide (lenx) and 69 blocks
       high (leny). It starts with 8 rows of stars which are
       followed by ten rows of empty space.
       The level is ended with 16 rows of solid bedrock.
       RGB is the color of the level.
       Below these 8 numbers is the definition of the level.
       This is what is drawn between the empty space and
       the bedrock.
     */
{
  int stat=1;
  word t1,t2,x,y,z;
  word sx,sy,sz;
  char *temp;
  buttondata *bdata;

  releasebana();
  
  powerplant=0;
  lenx=atoi(ptr[0]);
  if(lenx%BBILDX!=0)
    stat=0;
  lenx3=lenx<<3;
  leny=atoi(ptr[1]);
  sx=atoi(ptr[2]);
  for(sy=0; sy<3; sy++)
    if(sx<2*BBILDY) {
      sx+=BBILDY-(leny%BBILDY);
      leny+=BBILDY-(leny%BBILDY);
    }
  leny3=leny<<3;
  if(lenx*(long)leny>(long)maxlenx*maxleny)
    stat=0;
  sy=atoi(ptr[3]);
  sz=atoi(ptr[4]);
  x=y=lenx*(sx-2*BBILDY);
  if(!parsecolor(ptr[5], &bgcolor))
    stat=0;
  if(!parsecolor(ptr[6], &guncolor))
    stat=0;
  if(!parsecolor(ptr[7], &podcolor))
    stat=0;
  if(!parsecolor(ptr[8], &textcolor))
    stat=0;
  if(!parsecolor(ptr[9], &shieldcolor))
    stat=0;
  if(stat) {
    memset(bana,' ',lenx*(sx+sy));
    x>>=6;
    for(z=0; z<BBILDY; z++) {
      t1=random()%(BBILDY*lenx);
      t2=random()%16;
      *(bana+t1)=t2;
      *(bana+t1+BBILDY*lenx)=t2;
    }
    for(; x; x--)
      *(bana+2*BBILDY*lenx+(random()%y))=random()%16;
    for(y=sx+sy; y<leny-sz && stat; y++) {
      temp=ptr[y-sx-sy+10];
      for(x=0; x<lenx && stat; x++) {
	switch(temp[x]) {
	case '#':
	  if(nrbarriers<maxbarriers) {
	    barriers[nrbarriers].x=x;
	    barriers[nrbarriers++].y=y;
	  }
	  else
	    stat=0;
	  break;
	case '*':
	  if(nrrestartpoints<maxrestartpoints) {
	    restartpoints[nrrestartpoints].x=x;
	    restartpoints[nrrestartpoints++].y=y;
	  }
	  else
	    stat=0;
	  break;
	case '@':       /* Slide \block to the right */
	case 'A':       /* Slide \block to the left */
	case 'B':       /* Horizontal \blocker */
	case 'C':       /* Slide /block to the right */
	case 'D':       /* Slide /block to the left */
	case 'E':       /* Horizontal /blocker */
	case 'F':       /* Slide |block to the right */
	case 'G':       /* Slide |block to the left */
	case 'H':       /* Horizontal |blocker */
	case 'I':       /* Slide -block downwards */
	case 'J':       /* Slide -block upwards */
	case 'K':       /* Vertical -blocker */
	  if(nrsliders<maxsliders)
	    newslider(x,y,temp[x]-'@'+1);
	  else
	    stat=0;
	  break;
	case 'L':       /* Button on left wall */
	  if(nrthings<maxthings) {
	    bdata=malloc(sizeof(buttondata));
	    if(!bdata) {
	      printf("Out of memory.\n");
	      stat=0;
	    }
	    else {
	      bdata->sliders=NULL;
	      bdata->tag=temp[x-1];
	      bdata->major=ismajorbutton(bdata->tag);
	      newthing((x<<3)+9,(y<<3)+8,x,y,THING_BUTTON1,bdata);
	    }
	  }
	  else
	    stat=0;
	  break;
	case 'N':       /* Button on right wall */
	  if(nrthings<maxthings) {
	    bdata=malloc(sizeof(buttondata));
	    if(!bdata) {
	      printf("Out of memory.\n");
	      stat=0;
	    }
	    else {
	      bdata->sliders=NULL;
	      bdata->tag=temp[x-1];
	      bdata->major=ismajorbutton(bdata->tag);
	      newthing((x<<3)-1,(y<<3)+8,x,y,THING_BUTTON2,bdata);
	    }
	  }
	  else
	    stat=0;
	  break;
	case 'P':       /*  |\x Bunker */
	  if(nrthings<maxthings)
	    newthing((x<<3)+14,(y<<3)+8,x,y,THING_BUNKER1,NULL);
	  else
	    stat=0;
	  break;
	case 'U':       /*  x/| Bunker */
	  if(nrthings<maxthings)
	    newthing((x<<3)+10,(y<<3)+8,x,y,THING_BUNKER2,NULL);
	  else
	    stat=0;
	  break;
	case '[':       /*  |/x Bunker */
	  if(nrthings<maxthings)
	    newthing((x<<3)-2,(y<<3)+8,x-2,y,THING_BUNKER3,NULL);
	  else
	    stat=0;
	  break;
	case '\\':      /*  x\| Bunker */
	  if(nrthings<maxthings)
	    newthing((x<<3)+10,(y<<3)+7,x,y,THING_BUNKER4,NULL);
	  else
	    stat=0;
	  break;
	case '`':       /* Fuel */
	  if(nrthings<maxthings)
	    newthing((x<<3)+8,(y<<3)+8,x,y,THING_FUEL,NULL);
	  else
	    stat=0;
	  break;
	case 'd':       /* Power Plant */
	  if(nrthings<maxthings && !powerplant) {
	    powerplant=1;
	    ppblip=1;
	    ppx=x+2;
	    ppy=y;
	    newthing((x<<3)+12,(y<<3)+9,x,y,THING_POWERPLANT,NULL);
	  }
	  else
	    stat=0;
	  break;
	case 'm':
	  loadbx=x;
	  loadby=y;
	  break;
	}
	if(!stat)
	  printf("Unable to create a thing.\n");
      }
      memcpy(bana+(long)y*lenx,temp,lenx);
    }
    memset(bana+(long)y*lenx,'p',lenx*sz);
  }

  if(stat) {
    for(x=0; x<nrthings; x++) {
      if(things[x].type==7)
        *(bana+(long)things[x].py*lenx+things[x].px-1)='p';
      if(things[x].type==8)
        *(bana+(long)things[x].py*lenx+things[x].px-1)=' ';
      if(things[x].type>=3 && things[x].type<=6)
        things[x].mask = testbullets(&things[x]);
    }
    for(x=0; x<nrsliders; x++)
      *(bana+(long)sliders[x].y1*lenx+sliders[x].x1)='p';
    if(!matchsliders()) {
      printf("Unable to match all sliders.\n");
      stat=0;
    }
    if(!nrrestartpoints) {
      printf("There must be at least one restartpoint.\n");
      stat=0;
    }
    for(x=0; x<nrbarriers; x++) {
      barriers[x].restart=&restartpoints[0];
      for(y=1; y<nrrestartpoints; y++)
        if((abs(barriers[x].x-restartpoints[y].x)
            + abs(barriers[x].y-restartpoints[y].y))
           < (abs(barriers[x].x-barriers[x].restart->x)
              + abs(barriers[x].y-barriers[x].restart->y)))
          barriers[x].restart=&restartpoints[y];
    }
  }

  return(stat);
}
