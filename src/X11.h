
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef X11_H
#define X11_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#ifdef MITSHM
# include <sys/ipc.h>
# include <sys/shm.h>
# include <X11/extensions/XShm.h>
#endif /* MITSHM */

typedef struct {
  Pixmap pixmap;
  char *vbuff;
  XImage *ximage;
} VScreenType;

extern int icon48_nr_colors;
extern int icon48_cols;
extern int icon48_rows;
extern char icon48_colors[];
extern char icon48_pixels[];

void SetCustomPalette(void);
void CopyToScreen(void);
Pixmap create_icon(void);
#ifdef MITSHM
RETSIGTYPE signalerrorhandler(int num);
#endif /* MITSHM */

#endif
