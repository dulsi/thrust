
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

#define USED_COLORS (51)
extern ui8 color_conversion[USED_COLORS];
extern ui8 color_lookup[256];
extern ui8 palette_shift;
#define TEXTCOLOR     (color_lookup[0x20] + palette_shift)
#define HIGHLIGHT     (color_lookup[0x21] + palette_shift)
#define FUELCOLOR     (color_lookup[0x22] + palette_shift)
#define SCORETEXT     (color_lookup[0x23] + palette_shift)
#define SCORENAME     (color_lookup[0x24] + palette_shift)
#define BGCOLOR       (color_lookup[0x41] + palette_shift)
#define FUELHIGHLIGHT (color_lookup[0x88] + palette_shift)
#define STATUSFRAME   (color_lookup[0x88] + palette_shift)
#define STATUSNUMBERS (color_lookup[0x88] + palette_shift)
#define STATUSTEXT    (color_lookup[0x89] + palette_shift)
#define BLACK         (color_lookup[0x00] + palette_shift)
#define TRACTOR       (color_lookup[0x01] + palette_shift)
#define FRAGMENT      (color_lookup[0x0c] + palette_shift)
#define BULBS         (color_lookup[0x89] + palette_shift)
#define GUN           (color_lookup[0x8f] + palette_shift)
#define STAND         (color_lookup[0xa4] + palette_shift)
#define POD           (color_lookup[0xa5] + palette_shift)
#define SHIELD        (color_lookup[0xfe] + palette_shift)
#define SHIP          (color_lookup[0xff] + palette_shift)

#define SHIELDLIMIT SHIELD
#define CRASH_VALUE(x) (((ui8)(color_conversion[(x)-palette_shift]))>>5)

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
