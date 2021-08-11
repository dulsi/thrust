dnl Written by Peter Ekberg, peda@lysator.liu.se

AC_DEFUN([PE_PATH_GGI_INCLUDES],
[
AC_ARG_ENABLE(ggi-includes,
[  --enable-ggi-includes=DIR
                          ggi include files are in DIR],
[
  dnl User specified where includes are.
  if test "x$enableval" = x; then
    pe_cv_ggi_includes=NONE
  else
    pe_cv_ggi_includes="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_ggi_includes, [
    dnl User did not specify where includes are.
    dnl Check first if they are in the compiler include path.
    AC_TRY_CPP([
#include <ggi/ggi.h>
],
    [
      dnl Includes are in the compiler include path.
      pe_cv_ggi_includes=NONE
    ],
    [
      dnl Includes are not in the compiler include path.
      dnl Try to locate them.
      for pe_dir in                 \
        /usr/local/ggi/include      \
        /usr/local/include          \
        /usr/ggi/include            \
        /usr/include                \
        ; \
      do
        if test -r "$pe_dir/ggi/ggi.h" && \
           test -r "$pe_dir/ggi/gii.h"; then
          pe_cv_ggi_includes="$pe_dir"
          break
        fi
      done
    ])
    if test "x$pe_cv_ggi_includes" = x; then
      pe_cv_ggi_includes=no
    fi
  ])
])
])

AC_DEFUN([PE_PATH_GGI_LIBRARIES],
[
AC_ARG_ENABLE(ggi-libraries,
[  --enable-ggi-libraries=DIR
                          ggi library files are in DIR],
[
  dnl User specified where libraries are.
  if test "x$enableval" = x; then
    pe_cv_ggi_libraries=NONE
  else
    pe_cv_ggi_libraries="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_ggi_libraries, [
    dnl User did not specify where libraries are.
    pe_libs="$LIBS"
    LIBS="$LIBS -lggi"
    if test "$pe_cv_ggi_includes" != NONE; then
      pe_cflags="$CFLAGS"
      CFLAGS="-I$pe_cv_ggi_includes $CFLAGS"
    fi
    AC_TRY_LINK([#include <ggi/ggi.h>],
    [
      ggiInit();
    ],
    [
      dnl Libraries are found without -L{path}.
      pe_cv_ggi_libraries=NONE
    ],
    [
      dnl Libraries are not found without -L{path}.
      dnl Try to locate them.
      for pe_dir in             \
        /usr/local/ggi/lib      \
        /usr/local/lib/ggi      \
        /usr/local/lib          \
        /usr/ggi/lib            \
        /usr/lib/ggi            \
        /usr/lib                \
        /lib                    \
        ; \
      do
        if test -r "$pe_dir/libggi.so.2"; then
          pe_cv_ggi_libraries="$pe_dir"
          break
        fi
      done
      if test "x$pe_cv_ggi_libraries" = x; then
        pe_cv_ggi_libraries=no
      fi
    ])
    LIBS="$pe_libs"
    if test "$pe_cv_ggi_includes" != NONE; then
      CFLAGS="$pe_cflags"
    fi
  ])
])
])

AC_DEFUN([PE_PATH_GGI],
[
AC_MSG_CHECKING(for ggi)
AC_ARG_WITH(ggi,
  [  --with-ggi              use ggi, general graphics interface],
  [
    if test "x$withval" = xno; then
      no_ggi=yes
    fi
  ])
if test "x$no_ggi" = x; then
  PE_PATH_GGI_INCLUDES
  PE_PATH_GGI_LIBRARIES
  if test "$pe_cv_ggi_includes" != no && \
     test "$pe_cv_ggi_libraries" != no; then
    AC_MSG_RESULT(
     [headers $pe_cv_ggi_includes, libraries $pe_cv_ggi_libraries])
    if test "$pe_cv_ggi_includes" != NONE; then
      GGI_CFLAGS="-I$pe_cv_ggi_includes"
    fi
    if test "$pe_cv_ggi_libraries" != NONE; then
      GGI_LIBS="-L$pe_cv_ggi_libraries"
    fi
    AC_SUBST(GGI_CFLAGS)
    AC_SUBST(GGI_LIBS)

    pe_cppflags="$CPPFLAGS"
    CPPFLAGS="$GGI_CFLAGS $CPPFLAGS"
    AC_CHECK_HEADERS(ggi/wmh.h, [GGI_EXTRA_LIBS="-lggiwmh"])
    AC_CHECK_HEADERS(ggi/misc.h, [GGI_EXTRA_LIBS="$GGI_EXTRA_LIBS -lggimisc"])
    CPPFLAGS="$pe_cppflags"

    AC_SUBST(GGI_EXTRA_LIBS)
  else
    AC_MSG_RESULT(no)
    no_ggi=yes
  fi
else
  AC_MSG_RESULT(disabled)
fi
])
