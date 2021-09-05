#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conf.h"
#include "statistics.h"
#ifdef HAVE_GAMERZILLA
#include <gamerzilla.h>
#endif

int game_id = -1;

statistics stats;

static char *fields[6] = {
  "PlanetsDestroyed",
  "BunkersDestroyed",
  "Level",
  "PodsAcquired",
  "FuelAcquired",
  "FuelDestroyed"
};

void
clearstatgroup(statgroup *s)
{
  s->planetsdestroyed = 0;
  s->bunkersdestroyed = 0;
  s->level = 0;
  s->podsacquired = 0;
  s->fuelacquired = 0;
  s->fueldestroyed = 0;
}

void
parsestatline(char *line, statgroup *s)
{
  char *separator = strstr(line, " = ");
  if (separator) {
    long val = atol(separator + 3);
    if ((strncmp(line, fields[0], strlen(fields[0])) == 0) && (strlen(fields[0]) == separator - line))
      s->planetsdestroyed = val;
    else if ((strncmp(line, fields[1], strlen(fields[1])) == 0) && (strlen(fields[1]) == separator - line))
      s->bunkersdestroyed = val;
    else if ((strncmp(line, fields[2], strlen(fields[2])) == 0) && (strlen(fields[2]) == separator - line))
      s->level = val;
    else if ((strncmp(line, fields[3], strlen(fields[3])) == 0) && (strlen(fields[3]) == separator - line))
      s->podsacquired = val;
    else if ((strncmp(line, fields[4], strlen(fields[4])) == 0) && (strlen(fields[4]) == separator - line))
      s->fuelacquired = val;
    else if ((strncmp(line, fields[5], strlen(fields[5])) == 0) && (strlen(fields[5]) == separator - line))
      s->fueldestroyed = val;
  }
}

void
writestatgroup(FILE *f, statgroup *s)
{
  fprintf(f, "%s = %ld\n", fields[0], s->planetsdestroyed);
  fprintf(f, "%s = %ld\n", fields[1], s->bunkersdestroyed);
  fprintf(f, "%s = %ld\n", fields[2], s->level);
  fprintf(f, "%s = %ld\n", fields[3], s->podsacquired);
  fprintf(f, "%s = %ld\n", fields[4], s->fuelacquired);
  fprintf(f, "%s = %ld\n", fields[5], s->fueldestroyed);
}

void
initstatistics()
{
#ifdef HAVE_GAMERZILLA
  GamerzillaStart(false, get_user_path());
  char *data_path = get_data_path();
  char *game_file = malloc(strlen(data_path) + 35);
  strcpy(game_file, data_path);
  strcat(game_file, "gamerzilla/inertiablast.game");
  game_id = GamerzillaSetGameFromFile(game_file, data_path);
  free(game_file);
  free(data_path);
#endif
  clearstatgroup(&stats.overall);
  clearstatgroup(&stats.best);
  clearstatgroup(&stats.current);
  char *filename = getuserfile("statistics");
  FILE *f = fopen(filename, "rt");
  if (f) {
    char line[256];
    statgroup *s = NULL;
    while (!feof(f))
    {
      fgets(line, 256, f);
      if (line[0] == '[') {
        if (strncmp("Overall]", line + 1, 8) == 0)
          s = &stats.overall;
        else if (strncmp("Best]", line + 1, 5) == 0)
          s = &stats.best;
        else
          s = NULL;
      }
      else if (s != NULL)
        parsestatline(line, s);
    }
    fclose(f);
  }
  free(filename);
}

void
writestatistics()
{
  char *filename = getuserfile("statistics");
  FILE *f = fopen(filename, "wt");
  if (f) {
    fputs("[Overall]\n", f);
    writestatgroup(f, &stats.overall);
    fputs("\n[Best]\n", f);
    writestatgroup(f, &stats.best);
    fclose(f);
  }
  free(filename);
}

void
clearcurrentstats()
{
  clearstatgroup(&stats.current);
}

void
updatestatistics(long planets, long bunkers, long level, long pods, long fuelacquired, long fueldestroyed)
{
  stats.current.planetsdestroyed += planets;
  stats.current.bunkersdestroyed += bunkers;
  stats.current.level += level;
  stats.current.podsacquired += pods;
  stats.current.fuelacquired += fuelacquired;
  stats.current.fueldestroyed += fueldestroyed;
  stats.overall.planetsdestroyed += planets;
  stats.overall.bunkersdestroyed += bunkers;
  stats.overall.level += level;
  stats.overall.podsacquired += pods;
  stats.overall.fuelacquired += fuelacquired;
  stats.overall.fueldestroyed += fueldestroyed;
  if (stats.current.planetsdestroyed > stats.best.planetsdestroyed)
    stats.best.planetsdestroyed = stats.current.planetsdestroyed;
  if (stats.current.bunkersdestroyed > stats.best.bunkersdestroyed)
    stats.best.bunkersdestroyed = stats.current.bunkersdestroyed;
  if (stats.current.level > stats.best.level)
    stats.best.level = stats.current.level;
  if (stats.current.podsacquired > stats.best.podsacquired)
    stats.best.podsacquired = stats.current.podsacquired;
  if (stats.current.fuelacquired > stats.best.fuelacquired)
    stats.best.fuelacquired = stats.current.fuelacquired;
  if (stats.current.fueldestroyed > stats.best.fueldestroyed)
    stats.best.fueldestroyed = stats.current.fueldestroyed;
#ifdef HAVE_GAMERZILLA
  bool achieved;
  if ((pods > 0) && (GamerzillaGetTrophy(game_id, "First Pod", &achieved)) && (!achieved))
    GamerzillaSetTrophy(game_id, "First Pod");
  if ((pods > 0) && (stats.current.podsacquired == stats.current.level) && (GamerzillaGetTrophy(game_id, "Completed Normal Levels", &achieved)) && (!achieved))
    GamerzillaSetTrophyStat(game_id, "Completed Normal Levels", stats.current.level);
#endif
}
