dnl Written by Peter Ekberg, peda@lysator.liu.se

AC_DEFUN([PE_PATH_PBM_INCLUDES],
[
AC_ARG_ENABLE(pbm-includes,
[  --enable-pbm-includes=DIR
                          pbm include files are in DIR],
[
  dnl User specified where includes are.
  if test "x$enableval" = x; then
    pe_cv_pbm_includes=NONE
  else
    pe_cv_pbm_includes="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_pbm_includes, [
    dnl User did not specify where includes are.
    dnl Check first if they are in the compiler include path.
    AC_TRY_CPP([
#include <pnm.h>
],
    [
      dnl Includes are in the compiler include path.
      pe_cv_pbm_includes=NONE
    ],
    [
      dnl Includes are not in the compiler include path.
      dnl Try to locate them.
      for pe_dir in                 \
        /usr/local/pbm/include/X11  \
        /usr/local/pbm/include      \
        /usr/local/include/X11      \
        /usr/local/include          \
        /usr/pbm/include/X11        \
        /usr/pbm/include            \
        /usr/include/X11            \
        /usr/include                \
        ; \
      do
        if test -r "$pe_dir/pnm.h" && \
           test -r "$pe_dir/ppm.h" && \
           test -r "$pe_dir/pgm.h" && \
           test -r "$pe_dir/pbm.h"; then
          pe_cv_pbm_includes="$pe_dir"
          break
        fi
      done
    ])
    if test "x$pe_cv_pbm_includes" = x; then
      pe_cv_pbm_includes=no
    fi
  ])
])
])

AC_DEFUN([PE_PATH_PBM_LIBRARIES],
[
AC_ARG_ENABLE(pbm-libraries,
[  --enable-pbm-libraries=DIR
                          pbm library files are in DIR],
[
  dnl User specified where libraries are.
  if test "x$enableval" = x; then
    pe_cv_pbm_libraries=NONE
  else
    pe_cv_pbm_libraries="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_pbm_libraries, [
    dnl User did not specify where libraries are.
    pe_libs="$LIBS"
    LIBS="$LIBS -lpnm -lppm -lpgm -lpbm"
    if test "$pe_cv_pbm_includes" != NONE; then
      pe_cflags="$CFLAGS"
      CFLAGS="-I$pe_cv_pbm_includes $CFLAGS"
    fi
    AC_TRY_LINK([#include <pnm.h>],
    [
      pnm_allocarray(4, 6);
    ],
    [
      dnl Libraries are found without -L{path}.
      pe_cv_pbm_libraries=NONE
    ],
    [
      dnl Libraries are not found without -L{path}.
      dnl Try to locate them.
      for pe_dir in             \
        /usr/local/pbm/lib      \
        /usr/local/lib/pbm      \
        /usr/local/lib          \
        /usr/pbm/lib            \
        /usr/lib/pbm            \
        /usr/lib                \
        /lib                    \
        ; \
      do
        if test -r "$pe_dir/libpnm.so.1"; then
          pe_cv_pbm_libraries="$pe_dir"
          break
        fi
      done
      if test "x$pe_cv_pbm_libraries" = x; then
        pe_cv_pbm_libraries=no
      fi
    ])
    LIBS="$pe_libs"
    if test "$pe_cv_pbm_includes" != NONE; then
      CFLAGS="$pe_cflags"
    fi
  ])
])
])

AC_DEFUN([PE_PATH_OLDPBM],
[
AC_MSG_CHECKING(for pbm)
AC_ARG_WITH(pbm,
  [  --with-pbm              use pbm to rebuild datasrc/title.c],
  [
    if test "x$withval" = xno; then
      no_pbm=yes
    fi
  ])
if test "x$no_pbm" = x; then
  PE_PATH_PBM_INCLUDES
  PE_PATH_PBM_LIBRARIES
  if test "$pe_cv_pbm_includes" != no && \
     test "$pe_cv_pbm_libraries" != no; then
    AC_MSG_RESULT(
     [headers $pe_cv_pbm_includes, libraries $pe_cv_pbm_libraries])
    PBM_CFLAGS=
    PBM_LIBS=
    if test "$pe_cv_pbm_includes" != NONE; then
      PBM_CFLAGS="-I$pe_cv_pbm_includes"
    fi
    if test "$pe_cv_pbm_libraries" != NONE; then
      PBM_LIBS="-L$pe_cv_pbm_libraries -lppm -lpgm -lpbm"
    fi
  else
    AC_MSG_RESULT(no)
    no_pbm=yes
  fi
else
  AC_MSG_RESULT(disabled)
fi
])

AC_DEFUN([PE_PATH_NETPBM_INCLUDES],
[
AC_ARG_ENABLE(netpbm-includes,
[  --enable-netpbm-includes=DIR
                          netpbm include files are in DIR],
[
  dnl User specified where includes are.
  if test "x$enableval" = x; then
    pe_cv_netpbm_includes=NONE
  else
    pe_cv_netpbm_includes="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_netpbm_includes, [
    dnl User did not specify where includes are.
    dnl Check first if they are in the compiler include path.
    AC_TRY_CPP([
#include <pnm.h>
],
    [
      dnl Includes are in the compiler include path.
      pe_cv_netpbm_includes=NONE
    ],
    [
      dnl Includes are not in the compiler include path.
      dnl Try to locate them.
      netpbm-config --version >/dev/null 2>&1
      if test $? != 0; then 
        pe_dir=
      else
        pe_dir=`netpbm-config --includedir 2>/dev/null`
      fi
      if test -r "$pe_dir/pnm.h" && \
         test -r "$pe_dir/ppm.h" && \
         test -r "$pe_dir/pgm.h" && \
         test -r "$pe_dir/pbm.h"; then
        pe_cv_netpbm_includes="$pe_dir"
      fi
    ])
    if test "x$pe_cv_netpbm_includes" = x; then
      pe_cv_netpbm_includes=no
    fi
  ])
])
])

AC_DEFUN([PE_PATH_NETPBM_LIBRARIES],
[
AC_ARG_ENABLE(netpbm-libraries,
[  --enable-netpbm-libraries=DIR
                          netpbm library files are in DIR],
[
  dnl User specified where libraries are.
  if test "x$enableval" = x; then
    pe_cv_netpbm_libraries=NONE
  else
    pe_cv_netpbm_libraries="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_netpbm_libraries, [
    dnl User did not specify where libraries are.
    pe_libs="$LIBS"
    LIBS="$LIBS -lnetpbm"
    if test "$pe_cv_netpbm_includes" != NONE; then
      pe_cflags="$CFLAGS"
      CFLAGS="-I$pe_cv_netpbm_includes $CFLAGS"
    fi
    AC_TRY_LINK([#include <pnm.h>],
    [
      pnm_allocarray(4, 6);
    ],
    [
      dnl Libraries are found without -L{path}.
      pe_cv_netpbm_libraries=NONE
    ],
    [
      dnl Libraries are not found without -L{path}.
      dnl Try to locate them.
      netpbm-config --version >/dev/null 2>&1
      if test $? != 0; then 
        pe_cv_netpbm_libraries=
      else
        pe_cv_netpbm_libraries=`netpbm-config --linkdir 2>/dev/null`
      fi
      if test "x$pe_cv_netpbm_libraries" = x; then
        pe_cv_netpbm_libraries=no
      fi
    ])
    LIBS="$pe_libs"
    if test "$pe_cv_netpbm_includes" != NONE; then
      CFLAGS="$pe_cflags"
    fi
  ])
])
])

AC_DEFUN([PE_PATH_NETPBM],
[
AC_MSG_CHECKING(for netpbm)
AC_ARG_WITH(netpbm,
  [  --with-netpbm           use netpbm to rebuild datasrc/title.c],
  [
    if test "x$withval" = xno; then
      no_netpbm=yes
    fi
  ])
if test "x$no_netpbm" = x; then
  PE_PATH_NETPBM_INCLUDES
  PE_PATH_NETPBM_LIBRARIES
  if test "$pe_cv_netpbm_includes" != no && \
     test "$pe_cv_netpbm_libraries" != no; then
    AC_MSG_RESULT(
     [headers $pe_cv_netpbm_includes, libraries $pe_cv_netpbm_libraries])
    PBM_CFLAGS=
    PBM_LIBS=
    if test "$pe_cv_netpbm_includes" != NONE; then
      PBM_CFLAGS="-I$pe_cv_netpbm_includes"
    fi
    if test "$pe_cv_netpbm_libraries" != NONE; then
      PBM_LIBS="-L$pe_cv_netpbm_libraries"
    fi
  else
    AC_MSG_RESULT(no)
    no_netpbm=yes
  fi
else
  AC_MSG_RESULT(disabled)
fi
])

AC_DEFUN([PE_PATH_PBM],
[
no_netpbm=
no_pbm=
PE_PATH_NETPBM
if test "x$no_netpbm" = x; then
  PBM_LIBS="$PBM_LIBS -lnetpbm"
  no_pbm=yes
else
  # No netpbm, look for pbm...
  PE_PATH_OLDPBM
  if test "x$no_pbm" = x; then
    PBM_LIBS="$PBM_LIBS -lppm -lpgm -lpbm"
  fi
fi
AC_SUBST(PBM_CFLAGS)
AC_SUBST(PBM_LIBS)
])