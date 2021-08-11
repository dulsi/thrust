
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef GR_DRV_H
#define GR_DRV_H

#include "thrust_t.h"

#ifdef __STDC__
void clearscr(void);
void putarea(ui8 *source,
	     int x, int y, int width, int height, int bytesperline,
	     int destx, int desty);
void putpixel(int x, int y, ui8 color);
void syncscreen(unsigned long us);
void displayscreen(unsigned long us);
void fade_in(unsigned long us);
void fade_out(unsigned long us);
void fadepalette(int first, int last,
		 ui8 *RGBtable,
		 int fade, int flag);
void graphics_preinit(void);
int graphicsinit(int argc, char **argv);
int graphicsclose(void);
char *graphicsname(void);
#endif

#endif
