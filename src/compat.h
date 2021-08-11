
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef COMPAT_H
#define COMPAT_H

#ifndef HAVE_RANDOM
#define random rand
#define srandom srand
#endif /* HAVE_RANDOM */

#ifndef HAVE_USLEEP
void usleep(unsigned long usec);
#endif /* HAVE_USLEEP */

#ifndef HAVE_GETTIMEOFDAY
#ifndef _WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif
int gettimeofday(struct timeval *tv, void *);
#endif /* HAVE_GETTIMEOFDAY */

#ifndef HAVE_VGA_GETMODENUMBER
#define vga_getmodenumber __vga_name2number
#endif /* HAVE_VGA_GETMODENUMBER */

#ifndef HAVE_STRDUP
char *strdup(const char *s);
#endif /* HAVE_STRDUP */

#if defined(HAVE_GETOPT_H) && defined(HAVE_GETOPT_LONG_ONLY)
#include <getopt.h>
#elif !defined(HAVE_GETOPT_LONG_ONLY)
#include "../lib/getopt.h"
#endif

#ifdef HAVE_VALUES_H
#include <values.h>
#else
#include <limits.h>
#ifndef MAXINT
#define MAXINT  INT_MAX
#endif
#ifndef MAXLONG
#define MAXLONG LONG_MAX
#endif
#endif /* HAVE_VALUES_H */

#endif /* COMPAT_H */
