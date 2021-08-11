
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef HISCORE_H
#define HISCORE_H

#include "thrust_t.h"

#define HIGHSCORES (5)

typedef struct {
  char name[40];
  int score;
} highscoreentry;

extern highscoreentry highscorelist[HIGHSCORES];

#ifdef __STDC__
void writehighscores(void);
int readhighscores(void);
char *standardname(void);
int inithighscorelist(void);
int ahighscore(int score);
void inserthighscore(char *name, int score);
#endif

#endif /* HISCORE_H */
