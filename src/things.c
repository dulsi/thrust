
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdlib.h>
#include <math.h>

#include "compat.h"
#include "thrust_t.h"
#include "things.h"
#include "fast_gr.h"
#include "graphics.h"
#include "thrust.h"
#include "statistics.h"

#include "sound.h"

word nrthings=0;
word nrsliders=0;
word nrbarriers=0;
word nrrestartpoints=0;

bullet bullets[maxbullets];
fragment fragments[maxfragments];
thing things[maxthings];
slider sliders[maxsliders];
restartbarrier barriers[maxbarriers];
restartpoint restartpoints[maxrestartpoints];

word powerplant;
word ppx,ppy,ppcount;	/* Power Plant variables */
word ppblip;

void
newslider(int x, int y, int type)
{
  sliders[nrsliders].x1=x;
  sliders[nrsliders].y1=y;
  sliders[nrsliders].type=type;
  sliders[nrsliders].dir=type%3;
  sliders[nrsliders].match=0;
  sliders[nrsliders].active=0;
  sliders[nrsliders++].next=NULL;
}

int
majorbutton(int button)
{
  if(((buttondata *)things[button].data)->major==-1)
    return(button);
  else
    return(((buttondata *)things[button].data)->major);
}

void
newthing(int x, int y, int px, int py, int type, void *data)
{
  int life;

  switch(type) {
  case 1:       /* Fuel */
    life=57;
    break;
  default:
    life=2;
  }
  things[nrthings].alive=life;
  things[nrthings].x=x;
  things[nrthings].y=y;
  things[nrthings].px=px;
  things[nrthings].py=py;
  things[nrthings].type=type;
  things[nrthings].data=data;
  things[nrthings].mask=0xffff;
  things[nrthings++].score=0;
}

void
animatesliders(void)
{
  slider *s=sliders;
  word i;

  for(i=0; i<nrsliders; i++, s++)
    if(s->active) {
      switch(s->stage) {
      case 0:
        if(!(s->count&7)) {
          switch(s->type) {
          case 2:
            if(s->count==0) {
              writeblock(s->x1-(s->count>>3),   s->y1, 'x');
              writeblock(s->x1-(s->count>>3)-1, s->y1, 136);
              writeblock(s->x1-(s->count>>3)-2, s->y1, 'q');
            }
            else if(s->count==8) {
              writeblock(s->x1-(s->count>>3),   s->y1, 'w');
              writeblock(s->x1-(s->count>>3)-1, s->y1, 'r');
              writeblock(s->x1-(s->count>>3)-2, s->y1, 'q');
            }
            else {
              writeblock(s->x1-(s->count>>3),   s->y1, ' ');
              writeblock(s->x1-(s->count>>3)-1, s->y1, 'r');
              writeblock(s->x1-(s->count>>3)-2, s->y1, 'q');
            }
            break;
          case 5:
            if(s->count==0) {
              writeblock(s->x1-(s->count>>3)  , s->y1, 138);
              writeblock(s->x1-(s->count>>3)-1, s->y1, 139);
            }
            else if(s->count==8) {
              writeblock(s->x1-(s->count>>3)  , s->y1, 141);
              writeblock(s->x1-(s->count>>3)-1, s->y1, 134);
            }
            else {
              writeblock(s->x1-(s->count>>3)  , s->y1, 135);
              writeblock(s->x1-(s->count>>3)-1, s->y1, 134);
            }
            break;
          case 7:
            writeblock(s->x1+(s->count>>3), s->y1, 'y');
            break;
          case 8:
            writeblock(s->x1-(s->count>>3), s->y1, '|');
            break;
          case 10:
            writeblock(s->x1, s->y1+(s->count>>3), 129);
            break;
          case 11:
            writeblock(s->x1, s->y1-(s->count>>3), 128);
            break;
          }
        }
        else if(!(s->count&3)) {
          switch(s->type) {
          case 2:
            if(s->count==4) {
              writeblock(s->x1-(s->count>>3)-1, s->y1, 137);
              writeblock(s->x1-(s->count>>3)-2, s->y1, 'n');
            }
            else {
              writeblock(s->x1-(s->count>>3)-1, s->y1, 'o');
              writeblock(s->x1-(s->count>>3)-2, s->y1, 'n');
            }
            break;
          case 5:
            if(s->count==4) {
              writeblock(s->x1-(s->count>>3),   s->y1, 133);
              writeblock(s->x1-(s->count>>3)-1, s->y1, 140);
              writeblock(s->x1-(s->count>>3)-2, s->y1, 'u');
            }
            else if(s->count==12) {
              writeblock(s->x1-(s->count>>3),   s->y1, 132);
              writeblock(s->x1-(s->count>>3)-1, s->y1, 'v');
              writeblock(s->x1-(s->count>>3)-2, s->y1, 'u');
            }
            else {
              writeblock(s->x1-(s->count>>3),   s->y1, ' ');
              writeblock(s->x1-(s->count>>3)-1, s->y1, 'v');
              writeblock(s->x1-(s->count>>3)-2, s->y1, 'u');
            }
            break;
          case 7:
            writeblock(s->x1+(s->count>>3), s->y1, ' ');
            break;
          case 8:
            writeblock(s->x1-(s->count>>3), s->y1, ' ');
            break;
          case 10:
            writeblock(s->x1, s->y1+(s->count>>3), ' ');
            break;
          case 11:
            writeblock(s->x1, s->y1-(s->count>>3), ' ');
            break;
          }
        }
        s->count++;
        if(s->count >= ((abs(s->x1-s->x2)+abs(s->y1-s->y2)+1) << 3)-1) {
          s->count=0;
          s->stage=1;
        }
        break;
            case 1:
        s->count++;
        if(s->count>500) {
          s->count=0;
          s->stage=2;
        }
        break;
            case 2:
        if(!(s->count&7)) {
          switch(s->type) {
          case 2:
            if(s->count>>3==s->x1-s->x2) {
              writeblock(s->x2+(s->count>>3)-1, s->y1, 136);
              writeblock(s->x2+(s->count>>3)-2, s->y1, 'q');
            }
            else {
              writeblock(s->x2+(s->count>>3)-1, s->y1, 'r');
              writeblock(s->x2+(s->count>>3)-2, s->y1, 'q');
            }
            break;
          case 5:
            if(s->count>>3==s->x1-s->x2) {
              writeblock(s->x2+(s->count>>3)  , s->y1, 138);
              writeblock(s->x2+(s->count>>3)-1, s->y1, 139);
              writeblock(s->x2+(s->count>>3)-2, s->y1, 'p');
            }
            else if(s->count>>3==s->x1-s->x2-1) {
              writeblock(s->x2+(s->count>>3)  , s->y1, 141);
              writeblock(s->x2+(s->count>>3)-1, s->y1, 134);
              writeblock(s->x2+(s->count>>3)-2, s->y1, 'p');
            }
            else {
              writeblock(s->x2+(s->count>>3)  , s->y1, 135);
              writeblock(s->x2+(s->count>>3)-1, s->y1, 134);
              writeblock(s->x2+(s->count>>3)-2, s->y1, 'p');
            }
            break;
          case 7:
            writeblock(s->x2-(s->count>>3), s->y1, 'y');
            break;
          case 8:
            writeblock(s->x2+(s->count>>3), s->y1, '|');
            break;
          case 10:
            writeblock(s->x1, s->y2-(s->count>>3), 129);
            break;
          case 11:
            writeblock(s->x1, s->y2+(s->count>>3), 128);
            break;
          }
        }
        else if(!(s->count&3)) {
          switch(s->type) {
          case 2:
            if(s->count>>3==s->x1-s->x2) {
              writeblock(s->x2+(s->count>>3)  , s->y1, 'p');
              writeblock(s->x2+(s->count>>3)-1, s->y1, 'p');
              writeblock(s->x2+(s->count>>3)-2, s->y1, 'p');
            }
            else if(s->count>>3==s->x1-s->x2-1) {
              writeblock(s->x2+(s->count>>3)  , s->y1, 137);
              writeblock(s->x2+(s->count>>3)-1, s->y1, 'n');
              writeblock(s->x2+(s->count>>3)-2, s->y1, 'p');
            }
            else {
              writeblock(s->x2+(s->count>>3)  , s->y1, 'o');
              writeblock(s->x2+(s->count>>3)-1, s->y1, 'n');
              writeblock(s->x2+(s->count>>3)-2, s->y1, 'p');
            }
            break;
          case 5:
            if(s->count>>3==s->x1-s->x2) {
              writeblock(s->x2+(s->count>>3)  , s->y1, 'p');
              writeblock(s->x2+(s->count>>3)-1, s->y1, 'p');
            }
            else if(s->count>>3==s->x1-s->x2-1) {
              writeblock(s->x2+(s->count>>3)  , s->y1, 140);
              writeblock(s->x2+(s->count>>3)-1, s->y1, 'u');
            }
            else {
              writeblock(s->x2+(s->count>>3)  , s->y1, 'v');
              writeblock(s->x2+(s->count>>3)-1, s->y1, 'u');
            }
            break;
          case 7:
            writeblock(s->x2-(s->count>>3), s->y1, 'p');
            break;
          case 8:
            writeblock(s->x2+(s->count>>3), s->y1, 'p');
            break;
          case 10:
            writeblock(s->x1, s->y2-(s->count>>3), 'p');
            break;
          case 11:
            writeblock(s->x1, s->y2+(s->count>>3), 'p');
            break;
          }
        }
        s->count++;
        if(s->count >= ((abs(s->x1-s->x2)+abs(s->y1-s->y2)+1) << 3)-1)
          s->active=0;
        break;
      }
    }
}

void
startupsliders(int button)
{
  slider *s;

  s=((buttondata *)things[button].data)->sliders;

  while(s) {
    if(s->active) {
      switch(s->stage) {
      case 1:
        s->count=0;
        break;
      case 2:
        s->count=(((abs(s->x1-s->x2)+abs(s->y1-s->y2)+1) << 3)-1) - s->count-7;
        if(s->count<0)
          s->count=0;
        s->stage=0;
        break;
      default:
        break;
      }
    }
    else {
      if(play_sound)
        sound_play(SND_HARP, CHAN_3);
      s->active=1;
      s->stage=0;
      s->count=0;
    }
    s=s->next;
  }
}

restartpoint *
atbarrier(word bx, word by)
{
  word i;

  for(i=0; i<nrbarriers; i++)
    if((barriers[i].x==bx) && (barriers[i].y==by))
      return(barriers[i].restart);
  return(NULL);
}

void
deletething(thing *tp)
{
  int tx, ty, i, j;

  tx=(*tp).px;
  ty=(*tp).py;
  (*tp).alive=0;

  switch((*tp).type)
    {
    case 1:		/* Fuel */
      for(i=0; i<2; i++)
        for(j=0; j<2; j++)
          writeblock(tx+i, ty+j, ' ');
      break;
    case 2:		/* Power Plant */
      if(powerplant) {
        powerplant=0;
        countdown=800;
      }
      if((countdown&0x1f) == 0x10) {
        ui8 ch = 244;
        for(j=0; j<3; j++)
          for(i=0; i<3; i++)
            writeblock(tx+i, ty+j, ch++);
      }
      else if((countdown&0xf) == 0) {
        for(j=0; j<3; j++)
          for(i=0; i<3; i++)
            writeblock(tx+i, ty+j, ' ');
      }
      if(countdown)
        (*tp).alive=1;
      else {
      	invertedcolors();
        killallthings();
        if(play_sound)
          sound_play(SND_BOOM2, CHAN_4);
        for(j=0; j<3; j++)
          for(i=0; i<3; i++)
            writeblock(tx+i, ty+j, ' ');
        explodething(tp);
      }
    break;
    case 3:             /* Bunkers */
      writeblock(tx,   ty,   ' ');
      writeblock(tx+1, ty,   ' ');
      writeblock(tx+2, ty,   ' ');
      writeblock(tx+2, ty+1, ' ');
      break;
    case 4:
      writeblock(tx,   ty+1, ' ');
      writeblock(tx,   ty,   ' ');
      writeblock(tx+1, ty,   ' ');
      writeblock(tx+2, ty,   ' ');
      break;
    case 5:
      writeblock(tx,   ty+1, ' ');
      writeblock(tx+1, ty+1, ' ');
      writeblock(tx+2, ty+1, ' ');
      writeblock(tx+2, ty,   ' ');
      break;
    case 6:
      writeblock(tx,   ty,   ' ');
      writeblock(tx,   ty+1, ' ');
      writeblock(tx+1, ty+1, ' ');
      writeblock(tx+2, ty+1, ' ');
      break;
    case 7:
    case 8:
      writeblock(tx,   ty,   ' ');
      writeblock(tx,   ty+1, ' ');
      break;
    }
}

void
newbullet(word x, word y, int vx, int vy, word dir, int owner)
{
  static word nr=0;

  if(play_sound)
    sound_play(SND_BOOM, CHAN_2);
  bullets[nr].life=60;
  bullets[nr].x=x;
  bullets[nr].y=y;
  bullets[nr].vx=vx;
  bullets[nr].vy=vy;
  bullets[nr].dir=dir;
  bullets[nr++].owner=owner;

  if(nr==maxbullets)
    nr=0;
}

void
movebullets(void)
{
  int l;
  bullet *bulletptr;

  for(l=0, bulletptr=bullets; l<maxbullets; l++, bulletptr++)
    if((*bulletptr).life) {
      (*bulletptr).life--;
      (*bulletptr).x=((*bulletptr).x+(lenx<<6)+(*bulletptr).vx) % (lenx<<6);
      (*bulletptr).y=((*bulletptr).y+(leny<<6)-(*bulletptr).vy) % (leny<<6);
    }
}

word
crashtype(word type)
{
  switch(type) {
  case THING_FUEL:      /* Fuel */
  case THING_POWERPLANT:      /* Power Plant */
  case THING_BUNKER1:      /* 3-6 Bunkers */
  case THING_BUNKER2:
  case THING_BUNKER3:
  case THING_BUNKER4:
  case THING_BUTTON1:      /* 7-8 Buttons */
  case THING_BUTTON2:
    return(4);
  }
  return(0);
}

int
inloadcontact(int x, int y)
{
  int dx, dy;
  int dist;
  double angle;
  double sp;

  dx=x-(loadbx<<3)-4;
  dy=(loadby<<3)-y+9;
  dist=dx*dx+dy*dy;
  if(dist<34*34)
    return !loadcontact;

  if(!loadcontact)
    return 0;

  loadcontact=0;
  loaded=1;
  alpha = atan2(dy, dx);
  angle = atan2(speedy, speedx) - alpha;
  sp=hypot(speedx, speedy);
  deltaalpha=sp*sin(angle)/2 * M_PI/262144;
  deltaalpha=min(deltaalpha,  M_PI/16);
  deltaalpha=max(deltaalpha, -M_PI/16);
  speedx = (int)(speedx/(1 + REL_MASS));
  speedy = (int)(speedy/(1 + REL_MASS));
  return 0;
}

int
resonablefuel(int x, int y, int l)
{
  thing *tp;
  int dx, dy;

  tp=&things[l];
  dx=x-(int)(*tp).x;
  dy=(int)(*tp).y-y;
  if(dx>(int)(lenx3>>1))
    dx=lenx3-dx;
  if(-dx>(int)(lenx3>>1))
    dx=(word)(-(int)lenx3)+dx;
  return(dx>-10 && dx<9 && dy>5 && dy<60);
}

int
closestfuel(int x, int y)
{
  word i, which=0;
  thing *thingptr;
  int dx, dy;
  int minimum=MAXINT, d;
  int hit=0;

  for(i=0, thingptr=things; i<nrthings; i++, thingptr++)
    if((*thingptr).type==1 && (*thingptr).alive>1) {
      dx=abs((int)x-(int)(*thingptr).x);
      dy=abs((int)y-(int)(*thingptr).y);
      if(dx>(int)(lenx3>>1))
        dx-=lenx3;
      dy*=3;
      d=dx*dx+dy*dy;
      if(d<minimum) {
        minimum=d;
        which=i;
        hit=1;
      }
    }
  return(hit?which:-1);
}

int
closestbutton(int x, int y)
{
  word i, which=0;
  thing *thingptr;
  int dx, dy;
  int minimum=MAXINT, d;
  int hit=0;

  for(i=0, thingptr=things; i<nrthings; i++, thingptr++)
    if((*thingptr).type>=7 && (*thingptr).type<=8) {
      dx=abs((int)x-(int)(*thingptr).x);
      dy=abs((int)y-(int)(*thingptr).y);
      if(dx>(int)(lenx>>1))
        dx-=lenx;
      d=dx*dx+dy*dy;
      if(d<minimum) {
        minimum=d;
        which=i;
        hit=1;
      }
    }
  return(hit?which:-1);
}

void
hit(word x, word y, word crash, word owner)
{
  word i, which=0;
  thing *thingptr;
  long dx, dy;
  long minimum=MAXLONG, d;
  int hit=0;
  int kill=1;

  for(i=0, thingptr=things; i<nrthings; i++, thingptr++)
    if((*thingptr).alive>0 && crashtype((*thingptr).type)==crash) {
      dx=(int)x-(int)(*thingptr).x;
      dy=(int)y-(int)(*thingptr).y;
      d=(long)dx*dx+(long)dy*dy;
      if(d<minimum) {
        minimum=d;
        which=i;
        hit=1;
      }
    }
  if(hit) {
    if(play_sound)
      sound_play(SND_BOOM2, CHAN_4);
    explodething(&things[which]);
  }
  if(things[which].alive == 1)  /* Can't kill a dying thing */
    hit=0;
  if(hit) {
    switch(things[which].type) {
    case THING_POWERPLANT:
      if (ppblip == 0) {
        ui8 ch = 244;
        for(int j=0; j<3; j++)
          for(i=0; i<3; i++)
            writeblock(things[which].px+i, things[which].py+j, ch++);
      }
      ppblip+=10;
      if(ppblip>100)
        things[which].alive=1;  /* Dying */
      break;
    case THING_BUTTON1:                     /* Cannot kill buttons */
    case THING_BUTTON2:
      startupsliders(majorbutton(which));
      break;
    default:
      things[which].alive=1;	/* Dying */
      break;
    }
    if(kill) {
    }
    if(owner)
      switch(things[which].type) {
      case THING_FUEL:
        things[which].score=150;
        break;
      case THING_BUNKER1:
      case THING_BUNKER2:
      case THING_BUNKER3:
      case THING_BUNKER4:
        things[which].score=750;
        break;
      }
  }
}

word
testbullets(thing *b)
{
  word mask=0;
  int i, j, k;
  int dir=0;
  int basex=0, basey=0;
  int x, y;
  int vx, vy;
  int life;
  int tx, ty;
  ui8 blk, pix;
  ui8 res;

  switch((*b).type) {
  case 3:
    dir=-3;
    basex=((*b).x<<3)+14;
    basey=((*b).y<<3)-68;
    break;
  case 4:
    dir=3;
    basex=((*b).x<<3)-26;
    basey=((*b).y<<3)-68;
    break;
  case 5:
    dir=-14;
    basex=((*b).x<<3)+14;
    basey=((*b).y<<3)+44;
    break;
  case 6:
    dir=12;
    basex=((*b).x<<3)-26;
    basey=((*b).y<<3)+44;
    break;
  }
  dir+=16;
  for(i=0; i<16; ++i) {
    x=basex;
    y=basey;
    dir=(dir-1)&31;
    vx=(int)(24*cos(dir * M_PI/16));
    vy=(int)(24*sin(dir * M_PI/16));
    life=60;

    while(--life) {
      x=(x+(lenx<<6)+vx) % (lenx<<6);
      y=(y+(leny<<6)-vy) % (leny<<6);

      res=0;
      for(j=0;j<4;++j) {
      	ty=((y>>3)+j)%leny3;
        for(k=0;k<4;++k) {
          if(!*(bulletmap+((dir>>1)<<4)+j*4+k))
            continue;
          tx=((x>>3)+k)%lenx3;
          blk=*(bana+(long)(ty>>3)*lenx+(tx>>3));
          pix=*(blocks+(blk<<6)+((ty&7)<<3)+(tx&7));
          if(pix>res)
            res=pix;
        }
      }
      res=CRASH_VALUE(res);
      if(res)
        break;
    }
    mask=(mask<<1)|(res>=4);
  }
  return mask;
}

void
bunkerfirebullet(thing *b)
{
  int dir;

  dir=random()%16;
  if((1<<dir)&(*b).mask)
    return;
  switch((*b).type) {
  case 3:
    dir=(dir-3)&31;
    newbullet(((*b).x<<3)+14, ((*b).y<<3)-68,
	      (int)(24*cos(dir * M_PI/16)), (int)(24*sin(dir * M_PI/16)),
	      dir>>1, 0);
    break;
  case 4:
    dir=(dir+3)&31;
    newbullet(((*b).x<<3)-26, ((*b).y<<3)-68,
	      (int)(24*cos(dir * M_PI/16)), (int)(24*sin(dir * M_PI/16)),
	      dir>>1, 0);
    break;
  case 5:
    dir=(dir-14)&31;
    newbullet(((*b).x<<3)+14, ((*b).y<<3)+44,
	      (int)(24*cos(dir * M_PI/16)), (int)(24*sin(dir * M_PI/16)),
	      dir>>1, 0);
    break;
  case 6:
    dir=(dir+12)&31;
    newbullet(((*b).x<<3)-26, ((*b).y<<3)+44,
	      (int)(24*cos(dir * M_PI/16)), (int)(24*sin(dir * M_PI/16)),
	      dir>>1, 0);
    break;
  }
}

void
bunkerfirebullets(int intensity)
{
  word l;
  thing *thingptr;

  for(l=0, thingptr=things; l<nrthings; l++, thingptr++)
    if((*thingptr).alive)
      if((*thingptr).type>2 && (*thingptr).type<7)
	if((random()%1024) < 6 + intensity)
	  bunkerfirebullet(thingptr);
}

int
killdyingthings(int updatestats)
{
  word l;
  thing *thingptr;
  int res;
  long planets = 0;
  long bunkers = 0;
  long fuelacquired = 0;
  long fueldestroyed = 0;

  res=0;
  for(l=0, thingptr=things; l<nrthings; l++, thingptr++)
    if((*thingptr).alive==1) {
      switch(thingptr->type) {
        case THING_BUNKER1:
        case THING_BUNKER2:
        case THING_BUNKER3:
        case THING_BUNKER4:
          bunkers++;
          break;
        case THING_POWERPLANT:
          if (powerplant)
            planets++;
          break;
        case THING_FUEL:
          if (thingptr->score == 300)
            fuelacquired++;
          else
            fueldestroyed++;
          break;
        default:
          break;
      }
      deletething(thingptr);
      res+=(*thingptr).score;
    }

  if (updatestats)
    updatestatistics(planets, bunkers, 0, 0, fuelacquired, fueldestroyed);
  return(res);
}

void
killallthings(void)
{
  thing *thingptr;
  word l;

  for(l=0, thingptr=things; l<nrthings; l++, thingptr++) {
    if((*thingptr).alive > 1) {
      (*thingptr).alive=1;
      deletething(thingptr);
      explodething(thingptr);
    }
  }
}

void
newfragment(word x, word y)
{
  static word nr=0;
  int dir;
  int speed;

  dir=random()%32;
  speed=random()%48+16;
  fragments[nr].life=90-speed;
  fragments[nr].x=x;
  fragments[nr].y=y;
  fragments[nr].vx=(int)(speed*cos(dir * M_PI/16)/8);
  fragments[nr++].vy=(int)(speed*sin(dir * M_PI/16)/8);

  if(nr==maxfragments)
    nr=0;
}

void
explodething(thing *thingptr)
{
  int i;

  for(i=0; i<30; i++) {
    newfragment((thingptr->x<<3)-30+random()%61,
		(thingptr->y<<3)-30+random()%61);
  }
}

void
explodeship(void)
{
  int i;

  for(i=0; i<50; i++) {
    newfragment((x+((154+shipdx)<<3)+random()%61)%(lenx3<<3),
		(y+(( 82+shipdy)<<3)+random()%61)%(leny3<<3));
    if(loaded)
#if _MSC_VER >= 1000
    {
      /* Laugh attack! Internal compiler error on MSVC 5.0 SP3!!! */
      int xtmp, ytmp;
      xtmp = (x+((161-(int)((LOADLINELEN-loadpoint)*cos(alpha)/7.875))<<3)
		   +random()%61);
      xtmp %= (lenx3<<3);
      ytmp = (y+(( 89+(int)((LOADLINELEN-loadpoint)*sin(alpha)/7.875))<<3)
		   +random()%61);
      ytmp %= (leny3<<3);
      newfragment(xtmp, ytmp);
    }
#else
      newfragment((x+((161-(int)((LOADLINELEN-loadpoint)*cos(alpha)/7.875))<<3)
		   +random()%61)%(lenx3<<3),
		  (y+(( 89+(int)((LOADLINELEN-loadpoint)*sin(alpha)/7.875))<<3)
		   +random()%61)%(leny3<<3));
#endif
  }
}

void
movefragments(void)
{
  int l;
  fragment *fragmentptr;

  for(l=0, fragmentptr=&fragments[0]; l<maxfragments; l++, fragmentptr++)
    if((*fragmentptr).life>0) {
      (*fragmentptr).life--;
      (*fragmentptr).x=((*fragmentptr).x+(lenx<<6)+(*fragmentptr).vx)
        % (lenx<<6);
      (*fragmentptr).y=((*fragmentptr).y+(leny<<6)-(*fragmentptr).vy)
        % (leny<<6);
    }
}

word
livefragments(void)
{
  int l;
  fragment *fragmentptr;

  for(l=0, fragmentptr=&fragments[0]; l<maxfragments; l++, fragmentptr++)
    if(fragmentptr->life>0)
      return(1);
  return(0);
}

void
userestartpoint(restartpoint *restartxy, int loaded, int reverse, restartinfo *restart)
{
  restart->x=(lenx+restartxy->x-(154>>3))%lenx;
  restart->y=restartxy->y-(82>>3);
  restart->loaded=loaded;
}

void restorepowerplant(void)
{
  word i;
  thing *thingptr;

  for(i=0, thingptr=things; i<nrthings; i++, thingptr++)
    if((*thingptr).alive>0 && thingptr->type == THING_POWERPLANT) {
      ui8 ch = 'd';
      for(int j=0; j<3; j++)
        for(i=0; i<3; i++)
          writeblock(thingptr->px+i, thingptr->py+j, ch++);
    }
}
