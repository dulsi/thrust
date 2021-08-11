
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _WIN32

#include <sys/time.h>

int
gettimeofday(struct timeval *tv, void *)
{
}

#else

#include <Windows.h>

int
gettimeofday(struct timeval *tv, void *ptr)
{
  FILETIME ft;
  ULARGE_INTEGER uli;
  GetSystemTimeAsFileTime(&ft);
  uli.LowPart  = ft.dwLowDateTime;
  uli.HighPart = ft.dwHighDateTime;

  uli.QuadPart -= 0x019db1ded53e8000;
  tv->tv_sec  =  uli.QuadPart / 10000000;
  tv->tv_usec = (uli.QuadPart % 10000000) / 10;

  return 0;
}

#endif
