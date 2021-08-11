
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "../src/config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_IO_H
#include <io.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
  long j;
  int i,r,p=0;
  int ch;
  FILE *si, *so;
  
  if(argc != 2 && argc != 3) {
    fprintf(stderr, "%s: Usage %s [number] [pad]\n", argv[0], argv[0]);
    exit(1);
  }

  r=atoi(argv[1]);
  if(argc == 3)
    p=atoi(argv[2]);

#if defined(HAVE_FCNTL_H) && defined(HAVE_IO_H)
  setmode(fileno(stdin), O_BINARY);
  setmode(fileno(stdout), O_BINARY);
#endif

  si = fdopen(fileno(stdin), "rb");
  so = fdopen(fileno(stdout), "wb");

  if(fseek(si, 0L, SEEK_END))
    perror(argv[0]);
  for(j=ftell(si)/r-1; j>=0; j--) {
    if(fseek(si, j*r, SEEK_SET))
      perror(argv[0]);
    for(i=0; i<r-p; i++) {
      ch=fgetc(si);
      fputc(ch, so);
    }
  }
  
  return(0);
}
