
/* Written by Dennis Payne, dulsi@identicalsoftware.com */

#ifndef STATISTICS_H
#define STATISTICS_H

typedef struct {
  long planetsdestroyed;
  long bunkersdestroyed;
  long level;
  long podsacquired;
  long fuelacquired;
  long fueldestroyed;
} statgroup;

typedef struct {
  statgroup overall;
  statgroup best;
  statgroup current;
} statistics;

extern int game_id;

void initstatistics();
void writestatistics();
void clearcurrentstats();
void updatestatistics(long planets, long bunkers, long level, long pods, long fuelacquired, long fueldestroyed);

#endif
