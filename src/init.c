
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif
#include <math.h>

#include "thrust_t.h"
#include "keyboard.h"
#include "init.h"
#include "fast_gr.h"
#include "graphics.h"
#include "things.h"
#include "font5x5.h"
#include "thrust.h"
#include "gr_drv.h"

#include "sound.h"
sound_t snd[SOUND_SAMPLES];

void
turnship(void)
{
  word i,j,k;

  for(k=0; k<4; k++)
    for(i=0; i<16; i++)
      for(j=0; j<16; j++)
        *(ship+(5+k)*256 +     i*16+j)=
          *(ship+(3-k)*256 +(15-j)*16+15-i);
  for(i=0; i<4; i++) {
    memcpy(shipstorage,ship+(i<<8),256);
    memcpy(ship+(i<<8),ship+((8-i)<<8),256);
    memcpy(ship+((8-i)<<8),shipstorage,256);
  }
  for(k=0; k<8; k++)
    for(i=0; i<16; i++)
      for(j=0; j<16; j++)
        *(ship+(9+k)*256 +     i*16+j)=
          *(ship+(7-k)*256 +     i*16+15-j);
  for(k=0; k<15; k++)
    for(i=0; i<16; i++)
      for(j=0; j<16; j++)
        *(ship+(17+k)*256+     i*16+j)=
          *(ship+(15-k)*256+(15-i)*16+j);
}

void
makeshieldedship(void)
{
  word i,j,k;

  for(i=0; i<32; i++)
    for(j=0; j<17; j++)
      for(k=0; k<17; k++) {
        if(j&&k)
          *(shieldship+i*17*17+j*17+k)=*(ship+(i<<8)+((j-1)<<4)+k-1);
        else
          *(shieldship+i*17*17+j*17+k)=0;
        if(*(shld_pixels+j*17+k))
          *(shieldship+i*17*17+j*17+k)=*(shld_pixels+j*17+k) + shield_shift;
      }
}

void
makefuelmap(ui8 *fuelmap)
{
  int i;

  memset(fuelmap, 0, 2*4*32);

  for(i=0; i<32; i++) {
    *(fuelmap+4*(i+32)+(i>6)+(i>16)+(i>26)) = SHIP;
    *(fuelmap+4*i+(i<27)+(i<17)+(i<7)) = SHIP;
  }
}

int
initmem(void)
{
  word i;

  printf("Allocating memory...");
  fflush(stdout);

  bild=(ui8 *)malloc((long)PBILDX*PBILDY*2+16);
  bana=(ui8 *)malloc(maxlenx*maxleny);
  ship=(ui8 *)malloc(32*16*16);
  shieldship=(ui8 *)malloc(32*17*17);
  shipstorage=(ui8 *)malloc(17*17);
  bulletmap=(ui8 *)malloc(16*4*4);
  bulletstorage=(ui8 *)malloc(maxbullets*4*4);
  fragmentstorage=(ui8 *)malloc(maxfragments*2*2);
  fuelmap=(ui8 *)malloc(2*4*32);
  fuelstorage=(ui8 *)malloc(2*4*32);
  loadmap=(ui8 *)malloc(11*19);
  loadstorage=(ui8 *)malloc(11*19);
  wirestorage=(ui8 *)malloc(64);

  if(!bild || !bana || !ship || !shieldship || !shipstorage
     || !bulletmap || !bulletstorage || !fragmentstorage
     || !fuelmap || !fuelstorage || !loadmap || !loadstorage
     || !wirestorage) {
    printf("failed!.\n");
    fflush(stdout);
    return(0);
  }
  printf("done.\n");
  fflush(stdout);

  for(i=0; i<256; i++)
    pixel_collision[i] = 0;
  for (i = 0; i < blks_nr_colors; i++) {
    if ((blks_colors[i * 3] == 0) && (blks_colors[i * 3 + 1] == 0) && (blks_colors[i * 3 + 2] == 0)) {
      if (i == 0)
        break;
      blks_colors[i * 3] = blks_colors[0];
      blks_colors[i * 3 + 1] = blks_colors[1];
      blks_colors[i * 3 + 2] = blks_colors[2];
      blks_colors[0] = 0;
      blks_colors[1] = 0;
      blks_colors[2] = 0;
      for (int k = 0; k < blks_rows * blks_cols; k++) {
        if (blks_pixels[k] == i)
          blks_pixels[k] = 0;
        else if (blks_pixels[k] == 0)
          blks_pixels[k] = i;
      }
      break;
    }
  }
  memcpy(bin_colors, blks_colors, blks_nr_colors*3);
  for (i = 0; i < FIND_COLORS; i++) {
    for (int k = 0; k < blks_nr_colors; k++) {
      if ((blks_colors[k * 3] == findcolor[i * 3]) && (blks_colors[k * 3 + 1] == findcolor[i * 3 + 1]) && (blks_colors[k * 3 + 2] == findcolor[i * 3 + 2])) {
        foundcolor[i] = k;
        printf("Found %d at %d\n", i, k);
        break;
      }
    }
  }
  blocks=blks_pixels;
  for(i=39; i<256; i++) {
    if (i == 109)
      continue;
    if ((i >= 48) && (i <= 52))
      continue;
    for (int k = 0; k < 8 * 8; k++)
      if (blks_pixels[i * 8 * 8 + k] != 0) {
        if (i >=76 && i <= 108)
          pixel_collision[blks_pixels[i * 8 * 8 + k]] = 4;
        else if (pixel_collision[blks_pixels[i * 8 * 8 + k]] == 0)
          pixel_collision[blks_pixels[i * 8 * 8 + k]] = 2;
      }
  }
  bullet_shift = blks_nr_colors;
  memcpy(bin_colors + bullet_shift*3, bullet_colors, bullet_nr_colors*3);
  shield_shift = bullet_shift + bullet_nr_colors;
  memcpy(bin_colors + shield_shift*3, shld_colors, shld_nr_colors*3);
  ship_shift = shield_shift + shld_nr_colors;
  memcpy(ship, ship_pixels, 256*5);
  for(i=0; i<ship_cols*ship_rows; i++) {
    if (*(ship + i) > 0)
      *(ship+i) += ship_shift;
  }
  memcpy(bin_colors+ship_shift*3, ship_colors, ship_nr_colors*3);
  extracolor_shift = ship_shift + ship_nr_colors;
  memcpy(bin_colors + extracolor_shift*3, extracolor, EXTRA_COLORS * 3);
  for(i=0; i<16*4*4; i++)
    if (bullet_pixels[i] != 0) {
      bullet_pixels[i] += bullet_shift;
      pixel_collision[bullet_pixels[i]] = 1;
    }
  for(i=0; i<16; i++)
    memcpy(bulletmap+((20-i)&15)*16, bullet_pixels+i*16, 16);

  for(i=0; i<title_cols*title_rows; i++) {
    *(title_pixels+i) += extracolor_shift + EXTRA_COLORS;
  }

  memcpy(bin_colors+(extracolor_shift + EXTRA_COLORS)*3, title_colors, title_nr_colors*3);

  for(i=0; i<3*256; i++)
    bin_colors[i]=GAMMA(bin_colors[i]);

  printf("Turning the ship...");
  fflush(stdout);
  turnship();
  printf("done.\n");
  printf("Building graphics...");
  fflush(stdout);
  makefuelmap(fuelmap);
  for(i=0; i<3; i++) {
    memcpy(loadmap  + i    *11, blocks+64*'m'+(i+5)*8, 8);
    memcpy(loadmap+8+ i    *11, blocks+64*'0'+(i+5)*8, 3);
  }
  for(i=0; i<8; i++) {
    memcpy(loadmap  +(i+ 3)*11, blocks+64*'1'+i*8, 8);
    memcpy(loadmap+8+(i+ 3)*11, blocks+64*'2'+i*8, 3);
  }
  for(i=0; i<8; i++) {
    memcpy(loadmap  +(i+11)*11, blocks+64*'3'+i*8, 8);
    memcpy(loadmap+8+(i+11)*11, blocks+64*'4'+i*8, 3);
  }
  makeshieldedship();
  printf("done.\n");
  fflush(stdout);

/*  for(i=0; i<32*16*16; i++)
    *(ship+i) = color_lookup[*(ship+i)] + palette_shift;*/
/*  for(i=0; i<32*17*17; i++)
    *(shieldship+i) = color_lookup[*(shieldship+i)] + palette_shift;*/
/*  for(i=0; i<16*4*4; i++)
    *(bulletmap+i) += bullet_shift;*/
/*  for(i=0; i<11*19; i++)
    *(loadmap+i) = color_lookup[*(loadmap+i)] + palette_shift;*/
/*  for(i=0; i<2*4*32; i++)
    *(fuelmap+i) = color_lookup[*(fuelmap+i)] + palette_shift;*/
/*  for(i=0; i<title_cols*title_rows; i++)
    *(title_pixels+i) = color_lookup[*(title_pixels+i)] + palette_shift;*/
/*  for(i=0; i<256*8*8; i++)
    *(blocks+i) = color_lookup[*(blocks+i)] + palette_shift;*/

  printf("done.\n");
  fflush(stdout);

  chcolor=TEXTCOLOR;
  chpaper=BLACK;

  return 1;
}

void
inithardware(int argc, char **argv)
{
  int i;
	
  if(play_sound && !sound_depends_on_graphics()) {
    if(initsound())
      play_sound=0;
    else
      play_sound=1;
  }

  if(graphicsinit(argc, argv))
    exit(-1);

  if(play_sound && sound_depends_on_graphics()) {
    if(initsound())
      play_sound=0;
    else
      play_sound=1;
  }

  if(keyinit()) {
    printf("Failed to initialize the keyboard.\n");
    fflush(stdout);
    exit(-1);
  }
  printf("Keyboard initialized.\n");
  fflush(stdout);
}

void
initscreen(int round)
{
  int i,j;

  if(round&2)
    setcolor(BGCOLOR, 0);
  else
    setcolor(BGCOLOR, &bgcolor);

  setcolor(TRACTOR, &bgcolor);
  setcolor(BULBS,   &guncolor);
  setcolor(GUN,     &guncolor);
  setcolor(STAND,   &guncolor);
  setcolor(POD,     &podcolor);
  for (int i = 0; i < 4; i++) {
    color tmp = shieldcolor;
    shieldcolor.r = shieldcolor.r * (1.0 - (3 - i) / 10.0);
    shieldcolor.g = shieldcolor.g * (1.0 - (3 - i) / 10.0);
    shieldcolor.b = shieldcolor.b * (1.0 - (3 - i) / 10.0);
    setcolor(SHIELD + i,  &shieldcolor);
  }

  for(j=pblocky; j<BBILDY+pblocky; j++)
    for(i=pblockx; i<BBILDX+pblockx; i++)
      writeblock(i%lenx, j, *(bana+i%lenx+j*lenx));
}

void
initgame(int round, int reset, int xblock, int yblock)
{
  int i;

  crash=0;
  shoot=0;
#ifdef DEBUG
  repetetive=1;
#else
  repetetive=0;
#endif
  refueling=0;
  speedx=0;
  speedy=0;
  absspeed=0L;
  oldabs=0L;
  vx=0;
  vy=0;
  if(round&1) {
    kdir=72;
    dir=24;
    gravity=-20;
    alpha=3*M_PI/2;
    deltaalpha=0;
  }
  else {
    kdir=24;
    dir=8;
    gravity=20;
    alpha=M_PI/2;
    deltaalpha=0;
  }
  if(reset) {
    loaded=0;
    loadcontact=0;
    loadpoint=0;
    loadpointshift=0;
    shipdx=0;
    shipdy=0;
  }
  else {
    loadcontact=0;
    if(loaded) {
      loadpoint=LOADPOINT;
      loadpointshift=0;
      shipdx=(int)( cos(alpha)*loadpoint/5.90625);
      shipdy=(int)(-sin(alpha)*loadpoint/5.90625);
    }
    else {
      loadpoint=0;
      loadpointshift=0;
      *(bana+lenx* loadby   +loadbx  )='m';
      *(bana+lenx* loadby   +loadbx+1)='0';
      *(bana+lenx*(loadby+1)+loadbx  )='1';
      *(bana+lenx*(loadby+1)+loadbx+1)='2';
      *(bana+lenx*(loadby+2)+loadbx  )='3';
      *(bana+lenx*(loadby+2)+loadbx+1)='4';
      shipdx=0;
      shipdy=0;
    }
  }
  
  pblockx=xblock;
  pblocky=yblock+3*(round&1);
  if(loaded) {
    if(round&1)
      pblocky-=2;
    else
      pblocky+=2;
  }

  pixx=pblockx<<3;
  pixy=pblocky<<3;
  x=pixx<<3;
  y=pixy<<3;
  bildx=(pixx+PBILDX-4)%PBILDX+4;
  bildy=pixy%PBILDY;
  bblockx=bildx>>3;
  bblocky=bildy>>3;

  countdown=0;

  for(i=0; i<maxbullets; i++)
    bullets[i].life=0;
  for(i=0; i<maxfragments; i++)
    fragments[i].life=0;

  chcolor=TEXTCOLOR;
  chpaper=BLACK;
  chflag=BLACK;

  normalcolors();
}

int
initsound(void)
{
  printf("Initializing %s...", sound_name());
  fflush(stdout);

  snd[SND_BOOM  ].data = sound_boom;
  snd[SND_BOOM  ].len  = sound_boom_len;
  snd[SND_BOOM  ].loop = 0;
  snd[SND_BOOM2 ].data = sound_boom2;
  snd[SND_BOOM2 ].len  = sound_boom2_len;
  snd[SND_BOOM2 ].loop = 0;
  snd[SND_HARP  ].data = sound_harp;
  snd[SND_HARP  ].len  = sound_harp_len;
  snd[SND_HARP  ].loop = 0;
  snd[SND_ENGINE].data = sound_engine;
  snd[SND_ENGINE].len  = sound_engine_len;
  snd[SND_ENGINE].loop = 1;
  snd[SND_BLIP  ].data = sound_blip;
  snd[SND_BLIP  ].len  = sound_blip_len;
  snd[SND_BLIP  ].loop = 0;

  if(sound_init(snd)) {
    printf("No sound.\n");
    fflush(stdout);
    return(-1);
  }

  printf("done.\n");
  fflush(stdout);
  return(0);
}
                                                            
void
restoresound(void)
{
  printf("Restoring sound...");
  fflush(stdout);
  sound_exit();
  printf("done.\n");
  fflush(stdout);
}
         
void
restorehardware(void)
{
  printf("Releasing keyboard...");
  fflush(stdout);
  keyclose();
  printf("done.\n");
  fflush(stdout);

  if(play_sound && sound_depends_on_graphics())
    restoresound();

  graphicsclose();

  if(play_sound && !sound_depends_on_graphics())
    restoresound();
}

void
restoremem(void)
{
  printf("Freeing allocated memory...");
  fflush(stdout);
  free(bild);
  free(bana);
  free(bulletmap);
  free(ship);
  free(shieldship);
  free(shipstorage);
  free(fragmentstorage);
  free(wirestorage);
  free(fuelmap);
  free(fuelstorage);
  free(loadmap);
  free(loadstorage);
  printf("done.\n");
  fflush(stdout);
}
