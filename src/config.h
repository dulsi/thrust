/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* Have 16 bit sound */
/* #undef HAVE_16BIT_SOUND */

/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* Have Cygwin sound */
/* #undef HAVE_CYGWIN_SOUND */

/* vgakeyboard.h defines SCANCODE_P */
/* #undef HAVE_DEFINE_SCANCODE_P */

/* Have DirectSound */
/* #undef HAVE_DIRECTSOUND */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long_only' function. */
#define HAVE_GETOPT_LONG_ONLY 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <ggi/misc.h> header file. */
/* #undef HAVE_GGI_MISC_H */

/* Define to 1 if you have the <ggi/wmh.h> header file. */
/* #undef HAVE_GGI_WMH_H */

/* Have HP external speaker */
/* #undef HAVE_HP_EXTERNAL_SPEAKER */

/* Have HP internal speaker */
/* #undef HAVE_HP_INTERNAL_SPEAKER */

/* Have HP line out */
/* #undef HAVE_HP_LINE_OUT */

/* Have HP sound */
/* #undef HAVE_HP_SOUND */

/* Define to 1 if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Have OSS sound */
#define HAVE_LINUX_SOUND 1

/* Define to 1 if you have the <linux/soundcard.h> header file. */
#define HAVE_LINUX_SOUNDCARD_H 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `on_exit' function. */
#define HAVE_ON_EXIT 1

#ifndef _WIN32
/* Define to 1 if you have the `random' function. */
#define HAVE_RANDOM 1
#endif

/* Have sound */
#define HAVE_SOUND 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the <sun/audioio.h> header file. */
/* #undef HAVE_SUN_AUDIOIO_H */

/* Have SUN headphone */
/* #undef HAVE_SUN_HEADPHONE */

/* Have SUN line out */
/* #undef HAVE_SUN_LINE_OUT */

/* Have SUN sound */
/* #undef HAVE_SUN_SOUND */

/* Have SUN speaker */
/* #undef HAVE_SUN_SPEAKER */

/* Define to 1 if you have the <sys/audioio.h> header file. */
/* #undef HAVE_SYS_AUDIOIO_H */

/* Define to 1 if you have the <sys/audio.h> header file. */
/* #undef HAVE_SYS_AUDIO_H */

/* Define to 1 if you have the <sys/soundcard.h> header file. */
/* #undef HAVE_SYS_SOUNDCARD_H */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `usleep' function. */
#define HAVE_USLEEP 1

#ifndef _WIN32
/* values.h defines MAXINT */
#define HAVE_VALUES_H 1
#endif

/* svgalib has vga_getmodenumber function */
/* #undef HAVE_VGA_GETMODENUMBER */

/* Define to 1 if you have the <windows.h> header file. */
/* #undef HAVE_WINDOWS_H */

/* XShmPutImage in -lXext */
#define HAVE_XSHMPUTIMAGE 1

/* Full path of highscore file. */
#define HIGHSCOREFILE "/usr/local/com/thrust.highscore"

/* Name of package */
#define PACKAGE "inertiablast"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "dulsi@identicalsoftware.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "inertiablast"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "inertiablast 0.90"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "inertiablast"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.91"

/* foo */
#define PRINTF_RETURN 1

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Version number of package */
#define VERSION "0.91"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif
