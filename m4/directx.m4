dnl Written by Peter Ekberg, peda@lysator.liu.se

AC_DEFUN([PE_PATH_DIRECTX_INCLUDES],
[
AC_ARG_ENABLE(directx-includes,
[  --enable-directx-includes=DIR
                          DirectX include files are in DIR],
[
  dnl User specified where includes are.
  if test "x$enableval" = x; then
    pe_cv_directx_includes=NONE
  else
    pe_cv_directx_includes="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_directx_includes, [
    dnl User did not specify where includes are.
    dnl Check first if they are in the compiler include path.
    AC_TRY_CPP([
      #include <windows.h>
      #include <ddraw.h>
      #define DIRECTSOUND_VERSION 0x0700
      #define _LPCWAVEFORMATEX_DEFINED
      #include <dsound.h>
    ],
    [
      dnl Includes are in the compiler include path.
      pe_cv_directx_includes=NONE
    ],
    [
      dnl Includes are not in the compiler include path.
      dnl Try to locate them.
      for pe_dir in                 \
        /usr/local/directx/include      \
        /usr/local/include          \
        /usr/directx/include            \
        /usr/include                \
        ; \
      do
        if test -r "$pe_dir/ddraw.h" && \
           test -r "$pe_dir/dsound.h"; then
          pe_cv_directx_includes="$pe_dir"
          break
        fi
      done
    ])
    if test "x$pe_cv_directx_includes" = x; then
      pe_cv_directx_includes=no
    fi
  ])
])
])

AC_DEFUN([PE_PATH_DIRECTX_LIBRARIES],
[
AC_ARG_ENABLE(directx-libraries,
[  --enable-directx-libraries=DIR
                          DirectX library files are in DIR],
[
  dnl User specified where libraries are.
  if test "x$enableval" = x; then
    pe_cv_directx_libraries=NONE
  else
    pe_cv_directx_libraries="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_directx_libraries, [
    dnl User did not specify where libraries are.
    pe_libs="$LIBS"
    LIBS="$LIBS -lddraw -ldsound"
    if test "$pe_cv_directx_includes" != NONE; then
      pe_cflags="$CFLAGS"
      CFLAGS="-I$pe_cv_directx_includes $CFLAGS"
    fi
    AC_TRY_LINK(
    [
      #include <windows.h>
      #include <ddraw.h>
      #define DIRECTSOUND_VERSION 0x0700
      #define _LPCWAVEFORMATEX_DEFINED
      #include <dsound.h>
    ],
    [
      LPDIRECTDRAW lpDD;
      LPDIRECTSOUND lpDS;
      DirectDrawCreate(NULL, &lpDD, NULL);
      DirectSoundCreate(NULL, &lpDS, NULL);
    ],
    [
      dnl Libraries are found without -L{path}.
      pe_cv_directx_libraries=NONE
    ],
    [
      dnl Libraries are not found without -L{path}.
      dnl Try to locate them.
      for pe_dir in             \
        /usr/local/directx/lib      \
        /usr/local/lib/directx      \
        /usr/local/lib          \
        /usr/directx/lib            \
        /usr/lib/directx            \
        /usr/lib                \
        /lib                    \
        ; \
      do
        if test -r "$pe_dir/libddraw.a" && \
           test -r "$pe_dir/libdsound.a"; then
          pe_cv_directx_libraries="$pe_dir"
          break
        fi
      done
      if test "x$pe_cv_directx_libraries" = x; then
        pe_cv_directx_libraries=no
      fi
    ])
    LIBS="$pe_libs"
    if test "$pe_cv_directx_includes" != NONE; then
      CFLAGS="$pe_cflags"
    fi
  ])
])
])

AC_DEFUN([PE_PATH_DIRECTX],
[
AC_MSG_CHECKING(for directx)
AC_ARG_WITH(directx,
  [  --with-directx          use DirectX],
  [
    if test "x$withval" = xno; then
      no_directx=yes
    fi
  ])
if test "x$no_directx" = x; then
  PE_PATH_DIRECTX_INCLUDES
  PE_PATH_DIRECTX_LIBRARIES
  if test "$pe_cv_directx_includes" != no && \
     test "$pe_cv_directx_libraries" != no; then
    AC_MSG_RESULT(
     [headers $pe_cv_directx_includes, libraries $pe_cv_directx_libraries])
    if test "$pe_cv_directx_includes" != NONE; then
      DIRECTX_CFLAGS="-I$pe_cv_directx_includes"
    fi
    if test "$pe_cv_directx_libraries" != NONE; then
      DIRECTX_LIBS="-L$pe_cv_directx_libraries"
    fi
    if test "x$pe_sound" = xyes; then
      AC_DEFINE([HAVE_DIRECTSOUND], [1], [Have DirectSound])
      sound=yes
      AC_DEFINE([HAVE_SOUND], [1], [Have sound])
    fi
    AC_SUBST(DIRECTX_CFLAGS)
    AC_SUBST(DIRECTX_LIBS)
  else
    AC_MSG_RESULT(no)
    no_directx=yes
  fi
else
  AC_MSG_RESULT(disabled)
fi
])
