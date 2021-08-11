
/* Written by Peter Ekberg, peda@lysator.liu.se, after
   a long look at the source code for Koules version 1.3.
   Koules is also released under GPL with Copyrights owned
   by Jan Hubicka, hubicka@limax.paru.cas.cz. */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif
#include <string.h>
#include <sys/signal.h>
#include <sys/time.h>

#ifdef HAVE_XSHMPUTIMAGE
# define MITSHM
#endif

#include "compat.h"
#include "thrust.h"
#include "fast_gr.h"
#include "gr_drv.h"
#include "X11.h"
#include "options.h"
#include "init.h"

#ifdef MITSHM
XShmSegmentInfo shminfo;
char           *shared_mem;
int             shared_len;

static int haderror;
static int (*origerrorhandler)(Display *dp, XErrorEvent *event);
#endif /* MITSHM */

VScreenType framebuffer;

Display *dp;
Window wi;
int screen;
GC gc;
GC whitegc;
GC blackgc;
GC maskgc, orgc;
Colormap colormap;
int colordepth;
char *icondata;
int mitshm=1;
int double_size=0;

#define NKEEP 250

#ifdef MITSHM
static int
shmerrorhandler(Display *d, XErrorEvent *e)
{
  haderror++;
  if(e->error_code == BadAccess)
    printf("X failed to attach to shared memory.\n");
  else
    (*origerrorhandler)(d, e);
  return(0);
}

static void
shmfree(void)
{
  /*  uninitialized = 1;*/
  if(shmdt(shminfo.shmaddr) < 0)
    perror("Failed to detach from shared memory");
}

RETSIGTYPE
signalerrorhandler(int num)
{
  printf("Fatal signal %i received.\n", num);

  restoremem();
  restorehardware();

  /*  fadein();
  uninitialized = 1;*/

  exit(0);
}

static int
GetShmPixmap(VScreenType *pixmap)
{
  if(double_size)
    pixmap->ximage = XShmCreateImage(dp, DefaultVisual(dp, screen),
				     colordepth, ZPixmap, NULL, &shminfo,
				     PUSEX*2, (PUSEY + 24)*2);
  else
    pixmap->ximage = XShmCreateImage(dp, DefaultVisual(dp, screen),
				     colordepth, ZPixmap, NULL, &shminfo,
				     PUSEX, PUSEY + 24);
  if(!pixmap->ximage) {
    printf("Failed to get shared image.\n");
    return(0);
  }

  shared_len = (pixmap->ximage->bytes_per_line + 1)
    * (pixmap->ximage->height + 1);

  shminfo.shmid = shmget(IPC_PRIVATE, shared_len, IPC_CREAT | 0666);
  if(shminfo.shmid < 0) {
    perror("Failed to get shared memory");
    XDestroyImage(pixmap->ximage);
    return(0);
  }

  shminfo.shmaddr = (char *)shmat(shminfo.shmid, 0, 0);
  shared_mem = (char *)shminfo.shmaddr;
  if(shminfo.shmaddr == (char *)-1) {
    perror("Failed to attach to shared memory");
    XDestroyImage(pixmap->ximage);
    shmctl(shminfo.shmid, IPC_RMID, 0);
    return(0);
  }
  shminfo.readOnly = False;
  pixmap->ximage->data = (char *)shared_mem;

  /* Now try to attach it to the X Server */
  haderror = False;
  origerrorhandler = XSetErrorHandler(shmerrorhandler);
  if(!XShmAttach(dp, &shminfo)) {
    printf("X failed to attach to shared memory.\n");
    haderror++;
  }
  XSync(dp, False);		/* wait for error or ok */
  XSetErrorHandler(origerrorhandler);
  if(haderror) {
    if(shmdt(shminfo.shmaddr) < 0)
      perror("shmdt() error");
    if(shmctl(shminfo.shmid, IPC_RMID, 0) < 0)
      perror("shmctl rmid() error");
    return(0);
  }
  else {
    if(shmctl(shminfo.shmid, IPC_RMID, 0) < 0)
      perror("Failed to mark shared memory for removal");
  }

  memset((void *)shared_mem, 0, shared_len);

  pixmap->vbuff = shared_mem;

#if 0
  signal(SIGHUP, signalerrorhandler);
  signal(SIGINT, signalerrorhandler);
  signal(SIGTRAP, signalerrorhandler);
  signal(SIGABRT, signalerrorhandler);
  signal(SIGSEGV, signalerrorhandler);
  signal(SIGQUIT, signalerrorhandler);
  signal(SIGFPE, signalerrorhandler);
  signal(SIGTERM, signalerrorhandler);
  signal(SIGBUS, signalerrorhandler);
  signal(SIGIOT, signalerrorhandler);
  signal(SIGILL, signalerrorhandler);
#endif
#if defined(HAVE_ATEXIT)
  atexit(shmfree);
#elif defined(HAVE_ON_EXIT)
  on_exit(shmfree, 0);
#else
# error Do not know how to register a function to be called when exiting.
#endif

  return(1);
}
#endif /* MITSHM */

void
SetCustomPalette(void)
{
  int i;
  XColor c[256];
  int used_first;
  int used_entries=0;
  int entry;

  used_first = color_lookup[0];
  if(used_first == 0xff)
    printf("Aiee, bad fadepalette call.\n");
  for(i=0; i<256; i++) {
    entry = color_lookup[i];
    if(entry != 0xff) {
      c[entry].pixel = entry;
      c[entry].red   = (int)(bin_colors[i*3+0])<<8;
      c[entry].green = (int)(bin_colors[i*3+1])<<8;
      c[entry].blue  = (int)(bin_colors[i*3+2])<<8;
      c[entry].flags = DoRed | DoGreen | DoBlue;
      used_entries++;
    }
  }
  palette_shift=0;
  /*  XQueryColors(dp, DefaultColormap (dp, screen), c, NKEEP);*/
  XStoreColors(dp, colormap, c, USED_COLORS);
  XSetWindowColormap(dp, wi, colormap);
  XFlush(dp);
}

void
CopyToScreen(void)
{
#ifdef MITSHM
  if(mitshm) {
    if(double_size)
      XShmPutImage(dp, wi, whitegc, framebuffer.ximage,
		   0, 0, 0, 0, PUSEX*2, (PUSEY + 24)*2, True);
    else
      XShmPutImage(dp, wi, whitegc, framebuffer.ximage,
		   0, 0, 0, 0, PUSEX, PUSEY + 24, True);
  }
  else {
#endif /* MITSHM */
    XSetWindowBackgroundPixmap(dp, wi, framebuffer.pixmap);
    XClearWindow(dp, wi);
#ifdef MITSHM
  }
#endif /* MITSHM */
}

Pixmap
create_icon(void)
{
  XImage *img;
  Pixmap pixmap;
  int x, y, i;
  XColor iconcolors[5];

  if(icon48_nr_colors>5 || icon48_cols!=48 || icon48_rows!=48) {
    printf("Illegal icon. At most 5 colors and the size must be 48x48.\n");
    exit(1);
  }
  for(i=0; i<icon48_nr_colors; i++) {
    iconcolors[i].red   = 257*(ui8)icon48_colors[i*3  ];
    iconcolors[i].green = 257*(ui8)icon48_colors[i*3+1];
    iconcolors[i].blue  = 257*(ui8)icon48_colors[i*3+2];
    XAllocColor(dp, DefaultColormap(dp, screen), &iconcolors[i]);
  }

  img = XCreateImage(dp, DefaultVisual(dp, screen), DefaultDepth(dp, screen),
		     ZPixmap, 0, icondata, icon48_cols, icon48_rows,
		     BitmapPad(dp), 0);

  for(y = 0; y < icon48_rows; y++)
    for(x = 0; x < icon48_cols; x++)
      XPutPixel(img, x, y,
		iconcolors[(int)icon48_pixels[y*icon48_cols + x]].pixel);

  pixmap = XCreatePixmap(dp, wi, icon48_cols, icon48_rows,
			 DefaultDepth(dp, screen));
  XSync(dp, True);
  XPutImage(dp, pixmap, gc, img, 0, 0, 0, 0, icon48_cols, icon48_rows);
  XSync(dp, True);
  XDestroyImage(img);
  return(pixmap);
}

void
clearscr(void)
{
#ifdef MITSHM
  if(mitshm) {
    if(double_size)
      memset(framebuffer.vbuff, 0, PUSEX * 2 * (PUSEY + 24) * 2);
    else
      memset(framebuffer.vbuff, 0, PUSEX * (PUSEY + 24));
  }
  else {
#endif /* MITSHM */
    if(double_size)
      XFillRectangle(dp, framebuffer.pixmap, blackgc, 0, 0,
		     PUSEX*2, (PUSEY+24)*2);
    else
      XFillRectangle(dp, framebuffer.pixmap, blackgc, 0, 0, PUSEX, PUSEY+24);
#ifdef MITSHM
  }
#endif /* MITSHM */
  CopyToScreen();
}

void
putarea(ui8 *source,
	int x, int y, int width, int height, int bytesperline,
	int destx, int desty)
{
#ifdef MITSHM
  if(mitshm) {
    if(double_size) {
      int dy;
      int dx;
      ui8 *sptr;
      char *dptr;
      ui8 t;

      sptr = source + y*bytesperline + x;
      dptr = framebuffer.vbuff + desty*4*PUSEX + destx*2;

      for(dy=0;
	  dy<height;
	  dy++, sptr+=bytesperline-width, dptr+=4*PUSEX-2*width) {
	for(dx=0; dx<width; dx++) {
	  t = *(sptr++);
	  *(dptr+2*PUSEX) = t;
	  *(dptr++) = t;
	  *(dptr+2*PUSEX) = t;
	  *(dptr++) = t;;
	}
      }
    }
    else {
      int dy;
      ui8 *sptr;
      char *dptr;

      sptr = source + y*bytesperline + x;
      dptr = framebuffer.vbuff + desty*PUSEX + destx;

      for(dy=0; dy<height; dy++, sptr+=bytesperline, dptr+=PUSEX)
	memcpy(dptr, sptr, width);
    }
  }
  else {
#endif /* MITSHM */
    XImage *img;

    if(double_size) {
      static char *tmpdata = NULL;
      char *dptr;
      ui8 *sptr;
      int dx, dy;

      if(!tmpdata)
	tmpdata = malloc(4*PUSEX*(PUSEY+24));

      sptr = source + y*bytesperline + x;
      dptr = tmpdata;

      for(dy=0;
	  dy<height;
	  dy++, sptr+=bytesperline-width, dptr+=4*PUSEX-2*width) {
	for(dx=0; dx<width; dx++) {
	  *(dptr+2*PUSEX) = *(sptr);
	  *(dptr++) = *(sptr);
	  *(dptr+2*PUSEX) = *(sptr);
	  *(dptr++) = *(sptr++);
	}
      }

      img = XCreateImage(dp,
			 DefaultVisual(dp, screen), DefaultDepth(dp, screen),
			 ZPixmap, 0, (char *)tmpdata, width*2, height*2,
			 BitmapPad(dp), PUSEX*2);
      XPutImage(dp, framebuffer.pixmap, gc, img,
		0, 0, destx*2, desty*2, width*2, height*2);
    }
    else {
      img = XCreateImage(dp, DefaultVisual(dp, screen), DefaultDepth(dp, screen),
			 ZPixmap, 0, (char *)source + y*bytesperline + x,
			 width, height, BitmapPad(dp), bytesperline);
      XPutImage(dp, framebuffer.pixmap, gc, img,
		0, 0, destx, desty, width, height);
    }
    img->data=NULL;
    XDestroyImage(img);
#ifdef MITSHM
  }
#endif /* MITSHM */
}

void
putpixel(int x, int y, ui8 color)
{
#ifdef MITSHM
  if(mitshm) {
    if(double_size) {
      *(framebuffer.vbuff + (2*y  )*2*PUSEX + 2*x  ) = color;
      *(framebuffer.vbuff + (2*y  )*2*PUSEX + 2*x+1) = color;
      *(framebuffer.vbuff + (2*y+1)*2*PUSEX + 2*x  ) = color;
      *(framebuffer.vbuff + (2*y+1)*2*PUSEX + 2*x+1) = color;
    }
    else
      *(framebuffer.vbuff + y*PUSEX + x) = color;
  }
  else {
#endif /* MITSHM */
    XSetForeground(dp, gc, color);
    if(double_size) {
      XDrawPoint(dp, framebuffer.pixmap, gc, x*2  , y*2  );
      XDrawPoint(dp, framebuffer.pixmap, gc, x*2+1, y*2  );
      XDrawPoint(dp, framebuffer.pixmap, gc, x*2  , y*2+1);
      XDrawPoint(dp, framebuffer.pixmap, gc, x*2+1, y*2+1);
    }
    else
      XDrawPoint(dp, framebuffer.pixmap, gc, x, y);
#ifdef MITSHM
  }
#endif /* MITSHM */
}

static void
delaygame(long usec)
{
  static long extratime[3] = { 0, 0, 0 };
  struct timeval start, end, diff;
  long extradelay, delay;
  int i;

  extradelay = (extratime[0] + extratime[1] + extratime[2])/3;
  if(usec > extradelay) {
    gettimeofday(&start, NULL);
    usleep(usec - extradelay);
    gettimeofday(&end, NULL);
    diff.tv_sec = end.tv_sec - start.tv_sec;
    diff.tv_usec = end.tv_usec - start.tv_usec;
    if(diff.tv_usec < 0) {
      diff.tv_sec--;
      diff.tv_usec += 1000000;
    }
    delay = diff.tv_sec*1000000 + diff.tv_usec;
    extratime[0] = extratime[1];
    extratime[1] = extratime[2];
    extratime[2] = delay - (usec - extradelay);
  }
  else
    for(i=0; i<3; i++)
      extratime[i] = (extratime[i] * 9) / 10;
}

void
syncscreen(unsigned long us)
{
  struct timeval end, diff;
  static struct timeval start;
  static int first = 1;
  long delay;

  if(us)
    usleep(us);

  if(first) {
    gettimeofday(&start, NULL);
    first = 0;
  }

  XSync(dp, False);
  XFlush(dp);

  gettimeofday(&end, NULL);
  if(end.tv_sec < start.tv_sec ||
     (end.tv_sec == start.tv_sec &&
      end.tv_usec < start.tv_usec))
    end.tv_sec += 60*60*24;
  diff.tv_sec = end.tv_sec - start.tv_sec;
  diff.tv_usec = end.tv_usec - start.tv_usec;
  if(diff.tv_usec < 0) {
    diff.tv_sec--;
    diff.tv_usec += 1000000;
  }

  delay = 20000 - (diff.tv_sec*1000000 + diff.tv_usec);
  if(delay > 0) {
    delaygame(delay);
  }
  gettimeofday(&start, NULL);
}

void
displayscreen(unsigned long us)
{
  if(us)
    usleep(us);

  CopyToScreen();
  XFlush(dp);
}

void
fadepalette(int first, int last, ui8 *RGBtable, int fade, int flag)
{
  SetCustomPalette();
  if(flag)
    CopyToScreen();
}

void
fade_in(unsigned long us)
{
  int i=64;

  /*  for(i=1; i<=64; i++) */
    fadepalette(0, 255, bin_colors, i, 1);
  CopyToScreen();

  if(us)
    usleep(us);
}

void
fade_out(unsigned long us)
{
  int i=0;

  if(us)
    usleep(us);

  /*  for(i=64; i; i--)*/
    fadepalette(0, 255, bin_colors, i, 1);
  clearscr();
  usleep(500000L);
}



void
graphics_preinit(void)
{
}

int
graphicsinit(int argc, char **argv)
{
  XGCValues values;
  XWMHints   *wmhints;
  Atom delete_atom;
  XClassHint *classhints;
  XSizeHints *sizehints;
  XVisualInfo vinfo_return;
  XTextProperty windowName;
  XTextProperty iconName;
  char *title = "Thrust " PACKAGE_VERSION " by Peter Ekberg";
  char *title1 = "Thrust";
  char *Xdisplay = NULL;
  char *Xgeometry = NULL;
  int x, y, width, height, gravity, geometry;
  int optc;

  optind=0;
  do {
    static struct option longopts[] = {
      OPTS,
      X_OPTS,
      { 0, 0, 0, 0 }
    };

    optc=getopt_long_only(argc, argv, OPTC X_OPTC, longopts, (int *)0);
    switch(optc) {
    case 'm':
      mitshm = 0;
      break;
    case 'X':
      Xdisplay = strdup(optarg);
      break;
    case 'g':
      Xgeometry = strdup(optarg);
      break;
    case '2':
      double_size=1;
      break;
    }
  } while(optc != EOF);

  if((dp = XOpenDisplay(Xdisplay)) == 0) {
    printf("Could not open display\n");
    return(1);
  }
  XSetCloseDownMode(dp, DestroyAll);
  screen = DefaultScreen(dp);
  if(XMatchVisualInfo(dp,
		      DefaultScreen(dp),
		      8,
		      PseudoColor,
		      &vinfo_return)
     == False) {
    printf("Not 8 bits per pixel PseudoColor.\n");
    XCloseDisplay(dp);
    return(1);
  }
  colordepth = vinfo_return.depth;

#ifdef MITSHM
  if(mitshm) {
    if(!XShmQueryExtension(dp)) {
      printf("No support for shared memory. The game could be very slow.\n");
      mitshm=0;
    }
  }
  else
#endif /* MITSHM */
    printf("Not using shared memory. The game could be very slow.\n");

  if(double_size)
    wi = XCreateSimpleWindow(dp, RootWindow(dp, screen),
			     50, 50, PUSEX*2, (PUSEY + 24)*2, 0, 0, 0);
  else
    wi = XCreateSimpleWindow(dp, RootWindow(dp, screen),
			     50, 50, PUSEX, PUSEY + 24, 0, 0, 0);

  whitegc = XCreateGC(dp, wi, 0, NULL);
  blackgc = XCreateGC(dp, wi, 0, NULL);
  values.function = GXand;
  maskgc = XCreateGC(dp, wi, GCFunction, &values);
  values.function = GXor;
  orgc = XCreateGC(dp, wi, GCFunction, &values);
  gc = XCreateGC(dp, wi, 0, NULL);

  colormap = XCreateColormap(dp, wi, DefaultVisual(dp, screen), AllocAll);
/*  if(!XAllocColorCells(dp, colormap, True, NULL, 0, pixels, USED_COLORS)) {
    printf("Aieee.\n");
  }
  for(x = 0; x < USED_COLORS; ++x) {
    printf("x=%d, pix[x]=%d\n", x, pixels[x]);
  }*/

  SetCustomPalette();
  XSetWindowColormap(dp, wi, colormap);

  icondata=(char *)calloc((48+1+BitmapPad(dp)) * 48*sizeof(unsigned long), 1);
  if(icondata == NULL) {
    printf("Memory allocation error.\n");
    XCloseDisplay(dp);
    return(1);
  }

  wmhints = XAllocWMHints();
  classhints = XAllocClassHint();
  sizehints = XAllocSizeHints();

  if(wmhints==NULL || classhints==NULL || sizehints==NULL) {
    printf("Memory allocation error.\n");
    XCloseDisplay(dp);
    return(1);
  }

  wmhints->flags = StateHint | InputHint | IconPixmapHint;
  wmhints->initial_state = NormalState;
  wmhints->input = True;
  wmhints->icon_pixmap = create_icon();

  classhints->res_name = title1;
  classhints->res_class = title1;

  sizehints->flags = PSize | PMinSize | PMaxSize;
  if(double_size) {
    sizehints->min_width = PUSEX*2;
    sizehints->min_height = (PUSEY + 24)*2;
    sizehints->max_width = PUSEX*2;
    sizehints->max_height = (PUSEY + 24)*2;
  }
  else {
    sizehints->min_width = PUSEX;
    sizehints->min_height = PUSEY + 24;
    sizehints->max_width = PUSEX;
    sizehints->max_height = PUSEY + 24;
  }

  geometry = XWMGeometry(dp, screen, Xgeometry, NULL, 0, sizehints,
			 &x, &y, &width, &height, &gravity);

  if((geometry & (XValue | YValue)) == (XValue | YValue)) {
    sizehints->flags |= USPosition;
    sizehints->x = x;
    sizehints->y = y;
    XMoveWindow(dp, wi, x, y);
  }

  if(XStringListToTextProperty(&title, 1, &windowName) == 0) {
    printf("Cannot create window name resource.\n");
    XCloseDisplay(dp);
    return(1);
  }
  if(XStringListToTextProperty(&title1, 1, &iconName) == 0) {
    printf("Cannot create window name resource.\n");
    XCloseDisplay(dp);
    return(1);
  }

  XSetWMProperties(dp, wi, &windowName, &iconName,
		   argv, argc, sizehints, wmhints, classhints);
  XFree((void *)wmhints);
  XFree((void *)classhints);
  XFree((void *)sizehints);

  delete_atom = XInternAtom(dp, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(dp, wi, &delete_atom, 1);

  XSelectInput(dp, wi,
	       ExposureMask | KeyPressMask | KeyReleaseMask
	       | SubstructureRedirectMask | FocusChangeMask
	       | VisibilityChangeMask);

  XSetBackground(dp, whitegc, 0);
  XSetForeground(dp, whitegc, 0xffffffff);
  XSetForeground(dp, blackgc, 0);

  XSetBackground(dp, maskgc, 0xffffffff);
  XSetForeground(dp, maskgc, 0);

#ifdef MITSHM
  if(mitshm)
    if(!GetShmPixmap(&framebuffer)) {
      mitshm=0;
      printf("Shared memory disabled.\n");
    }
  if(!mitshm) {
#else
    printf("Shared memory disabled in config.h.\n");
#endif /* MITSHM */
    if(double_size)
      framebuffer.pixmap = XCreatePixmap(dp, wi, PUSEX*2, (PUSEY+24)*2,
					 DefaultDepth(dp, screen));
    else
      framebuffer.pixmap = XCreatePixmap(dp, wi, PUSEX, PUSEY+24,
					 DefaultDepth(dp, screen));
#ifdef MITSHM
  }
#endif /* MITSHM */

  XFlush(dp);
  clearscr();
  XFlush(dp);

  XMapWindow(dp, wi);

  return(0);
}

int
graphicsclose(void)
{
  XUnmapWindow(dp, wi);

#ifdef MITSHM
  if(mitshm)
    XDestroyImage(framebuffer.ximage);
  else
#endif /* MITSHM */
    XFreePixmap(dp, framebuffer.pixmap);
  XSync(dp, False);

#ifdef MITSHM
  if(mitshm) {
    XShmDetach(dp, &shminfo);
    XSync(dp, False);
  }
#endif /* MITSHM */

  XCloseDisplay(dp);

  return(0);
}

char *
graphicsname(void)
{
  static char name[] = "X11";

  return name;
}
