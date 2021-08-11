
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "thrust_t.h"
#include "hiscore.h"

highscoreentry highscorelist[HIGHSCORES];

void
writehighscores(void)
{
  FILE *fp;
  int i;

  fp=fopen(HIGHSCOREFILE, "wb");
  if(fp==NULL)
    return;
  for(i=0; i<HIGHSCORES; i++) {
    fwrite(highscorelist[i].name, 40, 1, fp);
    fputc(highscorelist[i].score    , fp);
    fputc(highscorelist[i].score>>8 , fp);
    fputc(highscorelist[i].score>>16, fp);
    fputc(highscorelist[i].score>>24, fp);
  }
  fclose(fp);
}

int
readhighscores(void)
{
  FILE *fp;
  int res;
  int i, j;

  fp=fopen(HIGHSCOREFILE, "rb");
  if(fp==NULL)
    return(0);

  for(i=0; i<HIGHSCORES; i++) {
    res = fread(highscorelist[i].name, 40, 1, fp);
    if(res != 1)
      return(0);
    highscorelist[i].score = 0;
    for(j=0; j<4; j++) {
      res = fgetc(fp);
      if(res == EOF)
	return(0);
      highscorelist[i].score += res<<(8*j);
    }
  }

  fclose(fp);
  return(1);
}

char *
standardname(void)
{
  char *tmp;
  static char name[40];

  tmp=getenv("USER");
  if(tmp==NULL)
    tmp=getenv("LOGNAME");
  if(tmp==NULL)
    tmp=getenv("USERNAME");
  if(tmp==NULL)
    name[0]=0;
  else {
    strncpy(name, tmp, 39);
    name[39]=0;
    name[0]=toupper(name[0]);
  }

  return(name);
}

int
inithighscorelist(void)
{
  int i;
  
  if(!readhighscores()) {
    for(i=0; i<HIGHSCORES; i++) {
      strcpy(highscorelist[i].name, "John Doe");
      highscorelist[i].score=(5-i)*1000;
    }
  }
  
  return(0);
}

int
ahighscore(int score)
{
  return(score>highscorelist[HIGHSCORES-1].score);
}

void
inserthighscore(char *name, int score)
{
  int i;

  for(i=HIGHSCORES; i>0 && score>highscorelist[i-1].score; i--) {
    if(i<HIGHSCORES) {
      strcpy(highscorelist[i].name, highscorelist[i-1].name);
      highscorelist[i].score = highscorelist[i-1].score;
    }
  }
  if(i<HIGHSCORES) {
    strcpy(highscorelist[i].name, name);
    highscorelist[i].score = score;
  }
}
