
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
  int skip;
  int ch;
  FILE *si, *so;

  if(argc != 2) {
    fprintf(stderr, "%s: Usage %s [number]\n", argv[0], argv[0]);
    exit(1);
  }

  skip=atoi(argv[1]);

#if defined(HAVE_FCNTL_H) && defined(HAVE_IO_H)
  setmode(fileno(stdin), O_BINARY);
  setmode(fileno(stdout), O_BINARY);
#endif

  si = fdopen(fileno(stdin), "rb");
  so = fdopen(fileno(stdout), "wb");

  if(fseek(si, skip, SEEK_SET))
    perror(argv[0]);
  for(;;) {
    ch=fgetc(si);
    if(ch==EOF)
      break;
    fputc(ch, so);
  }

  return(0);
}
