
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _WIN32

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/time.h>

void
usleep(unsigned long usec)
{
  struct timeval tv;

  tv.tv_sec  = usec / 1000000L;
  tv.tv_usec = usec % 1000000L;
  select(0, NULL, NULL, NULL, &tv);
}

#else

#include <Windows.h>

void
usleep(unsigned long usec)
{
  Sleep(usec / 1000);
}

#endif
