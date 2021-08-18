
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef THINGS_H
#define THINGS_H

#include "thrust_t.h"

struct bulletdef {
  int life,x,y,vx,vy,dir,owner;
};
typedef struct bulletdef bullet;

struct fragmentdef {
  int life,x,y,vx,vy;
};
typedef struct fragmentdef fragment;

struct thingdef	{
  int alive,px,py,x,y,type,score;
  word mask;
  void *data;
};
typedef struct thingdef thing;
extern word nrthings;

struct sliderdef {
  int type,x1,y1,x2,y2,dir,active,stage,count,match;
  struct sliderdef *next;
};
typedef struct sliderdef slider;
extern word nrsliders;

struct buttondatadef {
  int major;
  int tag;
  slider *sliders;
};
typedef struct buttondatadef buttondata;

struct restartpointdef {
  word x, y;
};
typedef struct restartpointdef restartpoint;
extern word nrrestartpoints;

struct barrierdef {
  word x, y;
  restartpoint *restart;
};
typedef struct barrierdef restartbarrier;
extern word nrbarriers;

struct restartinfodef {
  word x, y;
  int loaded;
};
typedef struct restartinfodef restartinfo;

#define THING_FUEL 1
#define THING_POWERPLANT 2
#define THING_BUNKER1 3
#define THING_BUNKER2 4
#define THING_BUNKER3 5
#define THING_BUNKER4 6
#define THING_BUTTON1 7
#define THING_BUTTON2 8

#define maxbullets (64)
extern bullet bullets[maxbullets];
#define maxfragments (512)
extern fragment fragments[maxfragments];
#define maxthings (32)
extern thing things[maxthings];
#define maxsliders (32)
extern slider sliders[maxsliders];
#define maxbarriers (512)
extern restartbarrier barriers[maxbarriers];
#define maxrestartpoints (16)
extern restartpoint restartpoints[maxrestartpoints];

extern word powerplant;
extern word ppx, ppy, ppcount;	/* Power Plant variables */
extern word ppblip;

#ifdef __STDC__
void newslider(int x, int y, int type);
int majorbutton(int button);
void newthing(int x, int y, int px, int py, int type, void *data);
void animatesliders(void);
void startupsliders(int button);
restartpoint *atbarrier(word bx, word by);
void deletething(thing *tp);
word testbullets(thing *b);
void newbullet(word x, word y, int vx, int vy, word dir, int owner);
void movebullets(void);
word crashtype(word type);
int inloadcontact(int x, int y);
int resonablefuel(int x, int y, int l);
int closestfuel(int x, int y);
int closestbutton(int x, int y);
void hit(word x, word y, word crash, word owner);
void bunkerfirebullet(thing *b);
void bunkerfirebullets(int intensity);
int killdyingthings(int updatestats);
void killallthings(void);
void newfragment(word x, word y);
void explodething(thing *thingptr);
void explodeship(void);
void movefragments(void);
word livefragments(void);
void userestartpoint(restartpoint *restartxy, int loaded,
	int reverse, restartinfo *restart);
#endif

#endif /* THINGS_H */
