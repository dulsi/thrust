
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef FAST_GR_H
#define FAST_GR_H

#include "thrust_t.h"

#define BBILDX	(41)
#define BBILDY	(23)
#define PBILDX	(BBILDX*8)
#define PBILDY	(BBILDY*8)
#define PUSEX	(PBILDX-8)
#define PUSEY	(PBILDY-8)

extern ui8 pixel_collision[256];

#define EXTRA_COLORS (12)
extern ui8 extracolor[EXTRA_COLORS * 3];
extern ui8 extracolor_shift;
#define TEXTCOLOR     (0 + extracolor_shift)
#define HIGHLIGHT     (1 + extracolor_shift)
#define FUELCOLOR     (2 + extracolor_shift)
#define SCORETEXT     (3 + extracolor_shift)
#define SCORENAME     (4 + extracolor_shift)
#define FUELHIGHLIGHT (5 + extracolor_shift)
#define STATUSFRAME   (6 + extracolor_shift)
#define STATUSNUMBERS (7 + extracolor_shift)
#define STATUSTEXT    (8 + extracolor_shift)
#define FRAGMENT      (9 + extracolor_shift)
#define SHIP          (10 + extracolor_shift)
#define TRACTOR       (11 + extracolor_shift)

#define BLACK         (0)
#define FIND_COLORS (5)
extern ui8 findcolor[FIND_COLORS * 3];
extern ui8 foundcolor[FIND_COLORS];
#define BGCOLOR       (foundcolor[0])
#define BULBS         (foundcolor[1])
#define GUN           (foundcolor[2])
#define STAND         (foundcolor[3])
#define POD           (foundcolor[4])

extern ui8 bullet_shift;
extern ui8 shield_shift;
extern ui8 ship_shift;
#define SHIELD        (shield_shift + 1)

#define SHIELDLIMIT 1
#define CRASH_VALUE(x) x

#ifdef __STDC__
void putscr(int x, int y);
void putblock(int x, int y, ui8 *source);
void drawfuel(int fuel);
void drawship(word bx, word by, ui8 *ship, ui8 *storage, int size);
void undrawship(word bx, word by, ui8 *storage, int size);
void drawsquare(word bx, word by,
		ui8 *object, ui8 *storage,
		ui8 deltax, ui8 deltay);
void undrawsquare(word bx, word by,
		  ui8 *storage,
		  ui8 deltax, ui8 deltay);
word testcrash(ui8 *object, ui8 *storage, word len, ui8 shield);
#endif

extern ui8 *bild;

#endif /* FAST_GR_H */
