
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <ggi/gg.h>
#include <ggi/ggi.h>
#ifdef HAVE_GGI_WMH_H
# include <ggi/wmh.h>
#endif
#ifdef HAVE_GGI_MISC_H
# include <ggi/misc.h>
#endif
#include <string.h>
#include <stdio.h>

#include "compat.h"
#include "thrust.h"
#include "fast_gr.h"
#include "gr_drv.h"
#include "options.h"

ggi_visual_t vis;
static ggi_color palette[256];
static int double_size;

#ifdef HAVE_GGI_MISC_H
static int use_ggi_misc_x = -1;
static int use_ggi_misc_y = -1;
#endif

#ifdef __CYGWIN__
#define WIN32
#define _WIN32
#define __WIN32
#define __WIN32__
#define WINNT
#endif

#ifdef _WIN32
#ifdef HAVE_WINDOWS_H
# include <windows.h>
#endif

HANDLE hWnd;

static BOOL CALLBACK
locate_wnd_callback(HWND wnd, LPARAM param)
{
  DWORD pid;
  GetWindowThreadProcessId(wnd, &pid);
  if(pid != GetCurrentProcessId())
    return TRUE;

#ifndef HAVE_GGI_WMH_H
  SetWindowText(wnd, "Thrust " PACKAGE_VERSION " by Peter Ekberg");
#endif /* HAVE_GGI_WMH_H */

  if(!hWnd)
    hWnd = wnd;
  return TRUE;
}
#endif /* _WIN32 */

static struct gg_task rate_limiter;

static int rate_limiter_task(struct gg_task *task)
{
  ggUnlock(task->hook);
  return 0;
}

static void
start_rate_limiter(void)
{
#ifdef HAVE_GGI_MISC_H
  if(use_ggi_misc_x != -1 && use_ggi_misc_y != -1)
    return;
#endif /* HAVE_GGI_MISC_H */

  memset(&rate_limiter, 0, sizeof(struct gg_task));
  rate_limiter.pticks = GG_SCHED_USECS2TICKS(16667); /* 60 Hz */
  if(rate_limiter.pticks == 0)
    rate_limiter.pticks = 1;
  if(rate_limiter.pticks >= GG_SCHED_TICK_WRAP)
    rate_limiter.pticks = GG_SCHED_TICK_WRAP - 1;
  rate_limiter.ncalls = 0;
  rate_limiter.hook = ggLockCreate();
  rate_limiter.cb = rate_limiter_task;
  ggAddTask(&rate_limiter);
}

static void
stop_rate_limiter(void)
{
#ifdef HAVE_GGI_MISC_H
  if(use_ggi_misc_x != -1 && use_ggi_misc_y != -1)
    return;
#endif /* HAVE_GGI_MISC_H */

  ggDelTask(&rate_limiter);
  ggUnlock(rate_limiter.hook);
  ggLockDestroy(rate_limiter.hook);
}

void
clearscr(void)
{
  ggiSetGCForeground(vis, ggiMapColor(vis, &palette[0]));
  ggiFillscreen(vis);
}

void
putarea(ui8 *source,
	int x, int y, int width, int height, int bytesperline,
	int destx, int desty)
{
  int dy;
  static ui8 buf[PUSEX*(PUSEY+24)*4];
  ui8 *src;
  ui8 *dst;

  if(double_size) {
    int dx;
    src = source + y*bytesperline + x;
    dst = buf;
    for(dy=y; dy<y+height; dst+=width*4, ++dy, src+=bytesperline) {
      for(dx=0; dx<width; ++dx) {
	*(dst+dx*2  ) = *(src+dx);
	*(dst+dx*2+1) = *(src+dx);
      }
//      memcpy(dst+width*2, dst, width*2);
    }
    ggiPutBox(vis, destx*2, desty*2, width*2, height*2, buf);
  }
  else {
    src = source + y*bytesperline + x;
    if(bytesperline != width) {
      dst = buf;
      for(dy=y; dy<y+height; dst+=width, ++dy, src+=bytesperline)
        memcpy(dst, src, width);
      src = buf;
    }
    ggiPutBox(vis, destx, desty, width, height, src);
  }
}

void
putpixel(int x, int y, ui8 color)
{
  color -= palette_shift;
  if(double_size) {
    ggiPutPixel(vis, x*2  , y*2  , ggiMapColor(vis, &palette[color]));
    ggiPutPixel(vis, x*2+1, y*2  , ggiMapColor(vis, &palette[color]));
//    ggiPutPixel(vis, x*2  , y*2+1, ggiMapColor(vis, &palette[color]));
//    ggiPutPixel(vis, x*2+1, y*2+1, ggiMapColor(vis, &palette[color]));
  }
  else
    ggiPutPixel(vis, x, y, ggiMapColor(vis, &palette[color]));
}

void
syncscreen(unsigned long us)
{
  if(us)
    usleep(us);

#ifdef HAVE_GGI_MISC_H
  if(use_ggi_misc_x != -1 && use_ggi_misc_y != -1) {
    ggiWaitRayPos(vis, &use_ggi_misc_x, &use_ggi_misc_y);
    return;
  }
#endif /* HAVE_GGI_MISC_H */

  ggLock(rate_limiter.hook);
}

void
displayscreen(unsigned long us)
{
  if(us)
    usleep(us);

  ggiFlush(vis);
}

void
fadepalette(int first, int last, ui8 *RGBtable, int fade, int flag)
{
  static int first_call=1;
  int entries,i;
  ggi_color *c;
  ui8 *d;
  int res;
  int used_first;
  int used_entries=0;

  entries=last-first+1;

  used_first = color_lookup[first];
  if(used_first == 0xff)
    printf("Aiee, bad fadepalette call.\n");
  c=&palette[used_first];
  d=RGBtable;
  i=0;

  fade = fade * 257 / 64;

  while(i<entries) {
    if(color_lookup[first+i] != 0xff) {
      c->r = *d++ * fade;
      c->g = *d++ * fade;
      c->b = *d * fade;
      c->a = 0;
      used_entries++;
    }
    else
      d += 2;
    if(++i<entries) {
      if(color_lookup[first+i-1] != 0xff)
        c++;
      d++;
    }
  }

  if(flag)
    ggiFlush(vis);

  if(first_call) {
    res=ggiSetPalette(vis, GGI_PALETTE_DONTCARE, used_entries, &palette[used_first]);
    palette_shift=res;
    first_call=0;
    return;
  }

  ggiSetPalette(vis, palette_shift, used_entries, &palette[used_first]);
}

void
fade_in(unsigned long us)
{
  int i;

  /*
  if(PSTARTY>0 && PSTARTX>0) {
    gl_hline(PSTARTX-3, PSTARTY-3, PSTARTX+PUSEX+2, 1);
    gl_hline(PSTARTX-4, PSTARTY-4, PSTARTX+PUSEX+3, 2);
    gl_hline(PSTARTX-5, PSTARTY-5, PSTARTX+PUSEX+4, 3);
    gl_hline(PSTARTX-3, PSTARTY+PUSEY+26, PSTARTX+PUSEX+2, 3);
    gl_hline(PSTARTX-4, PSTARTY+PUSEY+27, PSTARTX+PUSEX+3, 2);
    gl_hline(PSTARTX-5, PSTARTY+PUSEY+28, PSTARTX+PUSEX+4, 1);
    gl_line(PSTARTX-3, PSTARTY-3, PSTARTX-3, PSTARTY+PUSEY+26, 1);
    gl_line(PSTARTX-4, PSTARTY-4, PSTARTX-4, PSTARTY+PUSEY+27, 2);
    gl_line(PSTARTX-5, PSTARTY-5, PSTARTX-5, PSTARTY+PUSEY+28, 3);
    gl_line(PSTARTX+PUSEX+2, PSTARTY-3, PSTARTX+PUSEX+2, PSTARTY+PUSEY+26, 3);
    gl_line(PSTARTX+PUSEX+3, PSTARTY-4, PSTARTX+PUSEX+3, PSTARTY+PUSEY+27, 2);
    gl_line(PSTARTX+PUSEX+4, PSTARTY-5, PSTARTX+PUSEX+4, PSTARTY+PUSEY+28, 1);
  }
  else if(PSTARTY>0) {
    gl_hline(PSTARTX, PSTARTY-3, PSTARTX+PUSEX-1, 1);
    gl_hline(PSTARTX, PSTARTY-4, PSTARTX+PUSEX-1, 2);
    gl_hline(PSTARTX, PSTARTY-5, PSTARTX+PUSEX-1, 3);
    gl_hline(PSTARTX, PSTARTY+PUSEY+26, PSTARTX+PUSEX-1, 3);
    gl_hline(PSTARTX, PSTARTY+PUSEY+27, PSTARTX+PUSEX-1, 2);
    gl_hline(PSTARTX, PSTARTY+PUSEY+28, PSTARTX+PUSEX-1, 1);
  }
  */

  for(i = skip_frames?4:1; i <= 64; i += skip_frames?4:1)
    fadepalette(0, 255, bin_colors, i, 1);

  if(us)
    usleep(us);
}

void
fade_out(unsigned long us)
{
  int i;

  if(us)
    usleep(us);

  for(i = 64; i; i -= skip_frames?4:1)
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
  ggi_mode mode;
  char *ggitarget = NULL;
  char *palemu = NULL;
  int optc;

  optind=0;
  do {
    static struct option longopts[] = {
      OPTS,
      GGI_OPTS,
      { 0, 0, 0, 0 }
    };

    optc=getopt_long_only(argc, argv, OPTC GGI_OPTC, longopts, (int *)0);
    switch(optc) {
    case 't':
      ggitarget = strdup(optarg);
      break;
    case '2':
      double_size=1;
      break;
    }
  } while(optc != EOF);

  if(ggiInit()) {
    printf("GGI init failed.\n");
    return(-1);
  }

#ifdef HAVE_GGI_WMH_H
  if(ggiWmhInit()) {
    printf("GGI WMH init failed.\n");
    return(-1);
  }
#endif /* HAVE_GGI_WMH_H */

#ifdef HAVE_GGI_MISC_H
  if(ggiMiscInit()) {
    printf("GGI Misc init failed.\n");
    return(-1);
  }
#endif /* HAVE_GGI_MISC_H */

  if(ggitarget != NULL)
    vis = ggiOpen(ggitarget, NULL);
  else
    vis = ggiOpen(NULL);

  if(vis == NULL) {
    ggiExit();
    printf("Bad GGI target specified.\n");
    return(-1);
  }

  while(ggiCheckGraphMode(vis,
		       PUSEX*(double_size?2:1), (PUSEY+24)*(double_size?2:1),
		       GGI_AUTO, GGI_AUTO,
		       GT_8BIT, &mode)) {
    if(mode.visible.x < PUSEX*(double_size?2:1)
       || mode.visible.y < (PUSEY+24)*(double_size?2:1)
       || mode.graphtype != GT_8BIT) {
      /* Retry with palemu */
      ggiClose(vis);
      if(palemu == NULL) {
	if(ggitarget == NULL) {
	  ggitarget = getenv("GGI_DISPLAY");
	  if(ggitarget != NULL)
	    ggitarget = strdup(ggitarget);
	  else
	    ggitarget = strdup("");
	}
	palemu = (char *)malloc(18 + strlen(ggitarget));
	strcpy(palemu, "display-palemu:(");
	strcat(palemu, ggitarget);
	strcat(palemu, ")");

	vis = ggiOpen(palemu, NULL);
	if(vis == NULL) {
	  ggiExit();
	  printf("GGI display target fallback on palemu failed.\n");
	  return(-1);
	}
	continue;
      }
      ggiExit();
      printf("GGI graphics mode unusable.\n"
	     "Must be at least %dx%d pixels and must be 8 bits per pixel.\n",
	     PUSEX, PUSEY+24);
      return(-1);
    }
  }

  mode.frames = GGI_AUTO;
  mode.visible.x = PUSEX*(double_size?2:1);
  mode.visible.y = (PUSEY+24)*(double_size?2:1);
  mode.virt.x = GGI_AUTO;
  mode.virt.y = GGI_AUTO;
  mode.size.x = GGI_AUTO;
  mode.size.y = GGI_AUTO;
  mode.graphtype = GT_8BIT;
  mode.dpp.x = GGI_AUTO;
  mode.dpp.y = GGI_AUTO;

  if(ggiSetMode(vis, &mode)) {
    ggiClose(vis);
    ggiExit();
    printf("GGI set mode failed.\n");
    return(-1);
  }

  ggiAddFlags(vis, GGIFLAG_ASYNC);

#ifdef HAVE_GGI_WMH_H
  if(ggiWmhAttach(vis) >= 0)
    ggiWmhSetTitle(vis, "Thrust " PACKAGE_VERSION " by Peter Ekberg");
#endif /* HAVE_GGI_WMH_H */

#ifdef HAVE_GGI_MISC_H
  if(ggiMiscAttach(vis) >= 0) {
    use_ggi_misc_x = GGI_RP_DONTCARE;
    use_ggi_misc_y = GGI_RP_BORDER;
    if(ggiWaitRayPos(vis, &use_ggi_misc_x, &use_ggi_misc_y) < 0) {
      use_ggi_misc_x = -1;
      use_ggi_misc_y = -1;
    }
    if(use_ggi_misc_y == GGI_RP_DONTCARE) {
      use_ggi_misc_x = -1;
      use_ggi_misc_y = -1;
    }
  }
#endif /* HAVE_GGI_WMH_H */

  start_rate_limiter();

#ifdef _WIN32
  /* Try to locate the directx window, if any... */
  EnumWindows(locate_wnd_callback, 0);
#endif

  if(palemu)
    printf("GGI is emulating a palette, you can possibly get better performance\n"
	   "by setting the display to 256 colors.\n");

  fadepalette(0, 255, bin_colors, 1, 0);

  return 0;
}

int
graphicsclose(void)
{
  printf("Restoring textmode...");

  stop_rate_limiter();

#ifdef HAVE_GGI_WMH_H
  ggiWmhDetach(vis);
#endif /* HAVE_GGI_WMH_H */

  ggiClose(vis);

#ifdef HAVE_GGI_WMH_H
  ggiWmhExit();
#endif /* HAVE_GGI_WMH_H */

  ggiExit();
  printf("done.\n");

  return 0;
}

char *
graphicsname(void)
{
  static char name[] = "GGI";

  return name;
}
