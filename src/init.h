
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef INIT_H
#define INIT_H

#include "thrust_t.h"

#ifdef __STDC__
void turnship(void);
void makeshieldedship(void);
void makefuelmap(ui8 *fuelmap);
int initmem(void);
void inithardware(int argc, char **argv);
void initscreen(int round);
void initgame(int round, int reset, int xblock, int yblock);
int initsound(void);
void restoresound(void);
void restorehardware(void);
void restoremem(void);
#endif

#endif /* INIT_H */
