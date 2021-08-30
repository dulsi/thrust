
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef THRUST_H
#define THRUST_H

/* DEBUG  will make you undestructible.
   DEBUG2 gives you some trace output. */

/*#define DEBUG*/
/*#define DEBUG2*/

/* The speed of the game. Gravity and thrust is affected.
   Don't try too large values. 256 is what it used to
   be in the good old days. */

#define SPEED 230

/* The relative mass of the blob. I.e relative to the ship.
   Higher values makes it harder to lift.
   Try negative values at your own risk. */

#define REL_MASS 2.0

#define LOADLINELEN (268)
#define LOADPOINT ((int)(LOADLINELEN*REL_MASS/(REL_MASS + 1)))

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef HAVE_RANDOM
#define random rand
#define srandom srand
#endif /* HAVE_RANDOM */

#ifndef HAVE_USLEEP
void usleep(unsigned long usec);
#endif /* HAVE_USLEEP */

#include "thrust_t.h"

/* How many levels and what they are named. */

#define LEVELS (6)
extern char *level1[];
extern char *level2[];
extern char *level3[];
extern char *level4[];
extern char *level5[];
extern char *level6[];


extern ui8 sound_boom[];
extern ui8 sound_boom2[];
extern ui8 sound_harp[];
extern ui8 sound_engine[];
extern ui8 sound_blip[];
extern unsigned int  sound_boom_len;
extern unsigned int  sound_boom2_len;
extern unsigned int  sound_harp_len;
extern unsigned int  sound_engine_len;
extern unsigned int  sound_blip_len;

#define CHAN_1 0
#define CHAN_2 1
#define CHAN_3 2
#define CHAN_4 3
#define CHAN_5 4
#define SND_BOOM   0
#define SND_BOOM2  1
#define SND_HARP   2
#define SND_ENGINE 3
#define SND_BLIP   4

typedef enum { INST, PLAY, HI, ABOUT, END, CONF, NOTHING, DEMO } options;

extern unsigned int title_nr_colors;
extern unsigned int title_cols, title_rows;
extern ui8 title_colors[];
extern ui8 title_pixels[];
extern ui8 bin_blks[];
extern unsigned int ship_nr_colors;
extern unsigned int ship_cols, ship_rows;
extern ui8 ship_colors[];
extern ui8 ship_pixels[];
extern ui8 bin_shld[];
extern ui8 bin_colors[];
extern ui8 bin_bullet[];
extern ui8 bin_demomove[];

extern ui8 *bulletmap;
extern ui8 *blocks;
extern ui8 *ship;
extern ui8 *shieldship;
extern ui8 *bana;
extern ui8 *fuelmap;
extern ui8 *loadmap;
extern ui8 *shipstorage;
extern ui8 *bulletstorage;
extern ui8 *fragmentstorage;
extern ui8 *fuelstorage;
extern ui8 *loadstorage;
extern ui8 *wirestorage;

extern word lenx; /* Banans max i x-len, används senare till den aktuella */
extern word leny; /* Banans max i y-len, storleken på banan.Sätts i readbana */
extern word lenx3,leny3;

extern double alpha,deltaalpha;
extern word loaded,loadcontact,loadpointshift;
extern int loadpoint;
extern int countdown;
extern word crash,shoot,repetetive;
extern word refueling;
extern int speedx,speedy;
extern long absspeed,oldabs;
extern int kdir,dir;
extern int shipdx,shipdy;
extern int x,y;
extern int pixx,pixy;
extern int pblockx,pblocky;
extern int vx,vy;
extern int bildx,bildy;
extern int bblockx,bblocky;
extern int loadbx,loadby;
extern int gravity;
extern int score;
extern ui8 shield;
extern color bgcolor;
extern color guncolor;
extern color podcolor;
extern color textcolor;
extern color shieldcolor;
extern int play_sound;
extern double gamma_correction;
extern int skip_frames;

#define GAMMA(x)  ((int)(255*pow((x)/255.0, 1/gamma_correction)))

#ifdef __STDC__
int insideblock(int blockx, int blocky,
		int pblockx, int pblocky, int sx, int sy);
int insidepixel(int x, int y, int pixx, int pixy, int sx, int sy);
void updateborder(int pblockx, int pblocky,
		  int bblockx, int bblocky, int vx, int vy);
void pause_message(void);
void escape_message(void);
ui8 whatkeys(void);
ui8 nextmove(int reset);
void gamestatusframe(void);
void gamestatus(int lives, int fuel, int score);
int game(int demo);
void pressanykey(void);
int instructions(void);
int about(void);
char *enterhighscorename(void);
int showhighscores(void);
void newhighscore(void);
options menu(void);

#ifndef HAVE_GETTIMEOFDAY
#ifndef _WIN32
#include <sys/time.h>
#else
#include <Windows.h>
#endif
int gettimeofday(struct timeval *tv, void *);
#endif

#endif

#endif /* THRUST_H */
