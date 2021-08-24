dnl Written by Peter Ekberg, peda@lysator.liu.se
dnl Process this file with autoconf to produce a configure script.

AC_INIT([thrust],[0.89f],[dulsi@identicalsoftware.com])
AC_CONFIG_SRCDIR([src/thrust_t.h])
AC_CONFIG_MACRO_DIR(m4)
AM_INIT_AUTOMAKE([1.9 foreign])

AC_CONFIG_HEADER(src/config.h)

AM_MAINTAINER_MODE

AC_CANONICAL_HOST

no_ggi=
no_x=
no_directx=
no_svga=
no_netpbm=
no_pbm=
sound=

dnl Alternative programs
AC_PROG_CC
cflags=
optimize=
if test "x$GCC" = xyes; then
  cflags="$cflags -Wall -Wstrict-prototypes -Wmissing-prototypes"
  optimize="$optimize -fomit-frame-pointer -O9 -s"
else
  case "$host" in
    *hpux*)
      cflags="$cflags -Ae"
      optimize="$optimize +O4"
      ;;
  esac
fi
    
AC_PROG_INSTALL
AC_C_CONST
AC_C_INLINE
AC_PATH_PROG(POD2MAN, pod2man)
AC_PATH_PROG(POD2HTML, pod2html)
AC_ARG_VAR(POD2MAN, [pod2man command converts .pod to a man page])
AC_ARG_VAR(POD2HTML, [pod2html command converts .pod to html])

AC_PROG_RANLIB

dnl Libraries

dnl Library Functions
AC_CHECK_FUNCS(atexit on_exit)
PE_FUNC_PRINTF_RETURN

dnl Header Files
dnl AC_HEADER_STDC
AC_CHECK_HEADERS(getopt.h io.h fcntl.h windows.h malloc.h)
PE_VALUES_H_DEFINE_MAXINT

pe_sound=yes
AC_ARG_ENABLE(sound,
  [  --enable-sound          try to include sound],
  [
    if test "x$enableval" = xno; then
      pe_sound=no
    fi
  ])
if test "x$pe_sound" = xyes; then
  case "$host_os" in
    cygwin*)
      PE_CYGWIN_SOUND
      ;;
    linux*)
      PE_LINUX_SOUND
      ;;
    sun*)
      PE_SUN_SOUND
      ;;
    hpux*)
      PE_HP_SOUND
      ;;
  esac
fi

AC_CHECK_FUNCS(getopt_long_only, , [AC_LIBOBJ(getopt) AC_LIBOBJ(getopt1)])
AC_CHECK_FUNCS(random)
AC_REPLACE_FUNCS(usleep strdup gettimeofday)

need_compat=no
test "x$ac_cv_func_getopt_long_only" != "xyes" && need_compat=yes
test "x$ac_cv_func_usleep" != "xyes"           && need_compat=yes
test "x$ac_cv_func_strdup" != "xyes"           && need_compat=yes
test "x$ac_cv_func_gettimeofday" != "xyes"     && need_compat=yes

AM_CONDITIONAL(COMPAT, test x$need_compat = xyes)

dnl Structures
AC_TYPE_SIGNAL

dnl Typedefs

dnl System Services
PE_PATH_GGI

AC_PATH_X
AC_PATH_XTRA
AC_CHECK_LIB(Xext, XShmPutImage, AC_DEFINE([HAVE_XSHMPUTIMAGE], [1], [XShmPutImage in -lXext]), ,
             $X_PRE_LIBS $X_LIBS $X_EXTRA_LIBS -lX11)

PE_PATH_DIRECTX
AC_CHECK_TOOL(RC, windres,)
AC_ARG_VAR(RC, [Resource Compiler for Windows resources])

dnl Disable DirectX if there is no resource compiler
test "x$RC" = x && no_directx=yes

PE_PATH_SVGA
if test "x$no_svga" = x; then
  PE_HEADER_VGAKEYBOARD_DEFINE_P
  if test "x$no_svga" != x; then
    AC_MSG_WARN([SVGAlib disabled, could not find "vgakeyboard.h"])
  else
    PE_HEADER_VGA_HAS_VGA_GETMODENUMBER
  fi
fi

PE_PATH_PBM

dnl Output

if test "x$no_ggi$no_svga$no_x$no_directx" = xyesyesyesyes; then
  AC_MSG_WARN([No graphics backend found. Impossible to build game.])
fi

AM_CONDITIONAL(GGI_BACKEND,     test x$no_ggi = x)
AM_CONDITIONAL(X_BACKEND,       test x$no_x = x)
AM_CONDITIONAL(DIRECTX_BACKEND, test x$no_directx = x)
AM_CONDITIONAL(SVGA_BACKEND,    test x$no_svga = x)
AM_CONDITIONAL(DSOUND,          test "x$no_directx-$sound" = "x-yes")
AM_CONDITIONAL(SOUNDIT,
	test x$linux_sound$sun_sound$hp_sound$cygwin_sound = xyes)

dnl Bah! autoconf hasn't set prefix to it's default value yet!
test "x$prefix" = "xNONE" && prefix=$ac_default_prefix
test "x$exec_prefix" = "xNONE" && exec_prefix='${prefix}'

pe_old=
pe_sharedstatedir="$sharedstatedir"
until test "$pe_sharedstatedir" = "$pe_old"; do
  pe_old="$pe_sharedstatedir"
  eval pe_sharedstatedir="$pe_sharedstatedir"
done

AC_DEFINE_UNQUOTED(HIGHSCOREFILE, "$pe_sharedstatedir/thrust.highscore",
	[Full path of highscore file.])

AC_OUTPUT(
  Makefile
  lib/Makefile
  helpers/Makefile
  thrust-${PACKAGE_VERSION}.lsm:thrust.lsm
  datasrc/thrust_rc.rc:datasrc/thrust_rc.in)
