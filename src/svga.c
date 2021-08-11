
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <vga.h>
#include <vgagl.h>
#include <string.h>
#include <stdio.h>

#include "compat.h"
#include "thrust.h"
#include "fast_gr.h"
#include "gr_drv.h"
#include "options.h"

#define VGAMODE G320x200x256
#define PSTARTX ((WIDTH-PUSEX)>>1)
#define PSTARTY ((HEIGHT-PUSEY-24)>>1)

void
clearscr(void)
{
  gl_clearscreen(0);
}

void
putarea(ui8 *source,
	int x, int y, int width, int height, int bytesperline,
	int destx, int desty)
{
  ui8 *tmp;
  int res=PSTARTX+destx+WIDTH*(PSTARTY+desty);
  int dy, seg, lastseg, endseg;

  if(bytesperline==320 && width==320 && WIDTH==320 && x==0 && destx==0) {
    memcpy(graph_mem + desty*320, source + y*320, height*320);
  }
  else {
    tmp=source+y*bytesperline+x;

    seg=res>>16;
    vga_setpage(seg);
    lastseg = seg;
    for(dy=y; dy<y+height; res+=WIDTH, dy++, tmp+=bytesperline) {
      seg=res>>16;
      endseg=(res+width-1)>>16;
      if(seg != lastseg) {
	vga_setpage(seg);
	lastseg = seg;
      }
      if(seg == endseg)
	memcpy(graph_mem+(res&65535), tmp, width);
      else {
	int len = (endseg<<16)%WIDTH-PSTARTX-destx;
	memcpy(graph_mem+(res&65535), tmp, len);
	vga_setpage(endseg);
	lastseg = endseg;
	memcpy(graph_mem, tmp+len, width-len);
      }
    }
    vga_setpage(0);
  }
}

void
putpixel(int x, int y, ui8 color)
{
  gl_setpixel(PSTARTX+x, PSTARTY+y, color);
}

void
syncscreen(unsigned long us)
{
  if(us)
    usleep(us);
  vga_waitretrace();
}

void
displayscreen(unsigned long us)
{
  if(us)
    usleep(us);
}

void
fadepalette(int first, int last, ui8 *RGBtable, int fade, int flag)
{
  static int tmpRGBtable[768];
  int entries,i;
  int *c;
  ui8 *d;
  int used_first;
  int used_entries=0;

  entries=last-first+1;
  
  used_first = color_lookup[first];
  if(used_first == 0xff)
    printf("Aiee, bad fadepalette call.\n");
  c=&tmpRGBtable[3*used_first];
  d=RGBtable;
  i=0;

  while(i<entries) {
    if(color_lookup[first+i] != 0xff) {
      *c++ = (*d++ * fade) >> 8;
      *c++ = (*d++ * fade) >> 8;
      *c = (*d * fade) >> 8;
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
    vga_waitretrace();

  vga_setpalvec(used_first, used_entries, tmpRGBtable);
}

void
fade_in(unsigned long us)
{
  int i;

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

  for(i=1; i<=64; i++)
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

  for(i=64; i; i--)
    fadepalette(0, 255, bin_colors, i, 1);
  clearscr();
  usleep(500000L);
}




//int __vga_name2number(char *);

void
graphics_preinit(void)
{
#ifndef RUN_WITH_SVGA
#ifndef __DJGPP__
  uid_t ruid;
  gid_t rgid, egid;

  ruid = getuid();      /* Remember the real user */
  rgid = getgid();      /* Remember the real group */
  egid = getegid();     /* Remember the effective group */
  setreuid(0, -1);      /* Set the real user to root. Only possible if
                           already root or suid root which is a must for
                           svgalib programs */
#endif /* __DJGPP__ */
  vga_init();           /* Init svga, sets the effective user and group
                           to the real user and group */
#ifndef __DJGPP__
  setregid(rgid, egid); /* Restore the original real and effective group */
  setreuid(ruid, ruid); /* Restore the original real user and set the
                           effective user to the original real user,
                           throwing away the suid bit. */
#endif /* __DJGPP__ */
#endif /* RUN_WITH_SVGA */
}

int
graphicsinit(int argc, char **argv)
{
  int mode;
  vga_modeinfo *modeinfo;
  char *svgamode = NULL;
  int optc;

  optind=0;
  do {
    static struct option longopts[] = {
      OPTS,
      SVGA_OPTS,
      { 0, 0, 0, 0 }
    };

    optc=getopt_long_only(argc, argv, OPTC SVGA_OPTC, longopts, (int *)0);
    switch(optc) {
    case 's':
      svgamode = strdup(optarg);
      break;
    }
  } while(optc != EOF);

  if(svgamode != NULL) {
    mode = vga_getmodenumber(svgamode);
    if(mode==-1) {
      printf("Illegal Svgalib graphics mode specified.\n");
      return(-1);
    }
  }
  else
    mode = VGAMODE;

  modeinfo = vga_getmodeinfo(mode);
  /* This is necessary to make it not segfault under svgalib-dummy. */
  if(modeinfo == NULL) {
    printf("Unable to intialize Svgalib graphics.\n");
    return(-1);
  }
  if(modeinfo->width < PUSEX
     || modeinfo->height < PUSEY+24
     || modeinfo->bytesperpixel != 1
     || modeinfo->colors != 256) {
    printf("Svgalib graphics mode unusable.\n"
	   "Must be at least 320x200 pixels and must be one byte per pixel.\n");
    return(-1);
  }

  if(!vga_hasmode(mode)) {
    printf("Svgalib graphics mode is not available.\n");
    return(-1);
  }
  vga_setmode(mode);
  gl_setcontextvga(mode);

  fadepalette(0, 255, bin_colors, 1, 0);

  return 0;
}

int
graphicsclose(void)
{
#ifndef RUN_WITH_SVGA
  printf("Restoring textmode...");
  vga_setmode(TEXT);
  printf("done.\n");
#endif

  return 0;
}

char *
graphicsname(void)
{
  static char name[] = "SVGA";

  return name;
}
