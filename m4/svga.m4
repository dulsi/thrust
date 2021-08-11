dnl Written by Peter Ekberg, peda@lysator.liu.se

AC_DEFUN([PE_PATH_SVGA_INCLUDES],
[
AC_ARG_ENABLE(svga-includes,
[  --enable-svga-includes=DIR
                          SVGAlib include files are in DIR],
[
  dnl User specified where includes are.
  if test "x$enableval" = x; then
    pe_cv_svga_includes=NONE
  else
    pe_cv_svga_includes="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_svga_includes, [
    dnl User did not specify where includes are.
    dnl Check first if they are in the compiler include path.
    AC_TRY_CPP([
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>
],
    [
      dnl Includes are in the compiler include path.
      pe_cv_svga_includes=NONE
    ],
    [
      dnl Includes are not in the compiler include path.
      dnl Try to locate them.
      for pe_dir in             \
        /usr/local/svga/include \
        /usr/local/include      \
        /usr/include            \
        ; \
      do
        if test -r "$pe_dir/vga.h" && \
           test -r "$pe_dir/vgagl.h" && \
           test -r "$pe_dir/vgakeyboard.h"; then
          pe_cv_svga_includes="$pe_dir"
          break
        fi
      done
    ])
    if test "x$pe_cv_svga_includes" = x; then
      pe_cv_svga_includes=no
    fi
  ])
])
])

AC_DEFUN([PE_PATH_SVGA_LIBRARIES],
[
AC_ARG_ENABLE(svga-libraries,
[  --enable-svga-libraries=DIR
                          SVGAlib library files are in DIR],
[
  dnl User specified where libraries are.
  if test "x$enableval" = x; then
    pe_cv_svga_libraries=NONE
  else
    pe_cv_svga_libraries="$enableval"
  fi
],
[
  AC_CACHE_VAL(pe_cv_svga_libraries, [
    dnl User did not specify where libraries are.
    pe_libs="$LIBS"
    LIBS="$LIBS -lvga"
    if test "$pe_cv_svga_includes" != NONE; then
      pe_cflags="$CFLAGS"
      CFLAGS="-I$pe_cv_svga_includes $CFLAGS"
    fi
    AC_TRY_LINK([#include <vga.h>],
    [
      vga_init();
    ],
    [
      dnl Libraries are found without -L{path}.
      pe_cv_svga_libraries=NONE
    ],
    [
      dnl Libraries are not found without -L{path}.
      dnl Try to locate them.
      for pe_dir in             \
        /usr/local/svga/lib     \
        /usr/local/lib/svga     \
        /usr/local/lib          \
        /usr/lib                \
        /lib                    \
        ; \
      do
        if test -r "$pe_dir/libvga.so.1"; then
          pe_cv_svga_libraries="$pe_dir"
          break
        fi
      done
      if test "x$pe_cv_svga_libraries" = x; then
        pe_cv_svga_libraries=no
      fi
    ])
    LIBS="$pe_libs"
    if test "$pe_cv_svga_includes" != NONE; then
      CFLAGS="$pe_cflags"
    fi
  ])
])
])

AC_DEFUN([PE_PATH_SVGA],
[
AC_MSG_CHECKING(for SVGAlib)
AC_ARG_WITH(svga,
  [  --with-svga             use SVGAlib],
  [
    if test "x$withval" = xno; then
      no_svga=yes
    fi
  ])
if test "x$no_svga" = x; then
  PE_PATH_SVGA_INCLUDES
  PE_PATH_SVGA_LIBRARIES
  if test "$pe_cv_svga_includes" != no && \
     test "$pe_cv_svga_libraries" != no; then
    AC_MSG_RESULT(
     [headers $pe_cv_svga_includes, libraries $pe_cv_svga_libraries])
    if test "$pe_cv_svga_includes" != NONE; then
      SVGA_CFLAGS="-I$pe_cv_svga_includes"
    fi
    if test "$pe_cv_svga_libraries" != NONE; then
      SVGA_LIBS="-L$pe_cv_svga_libraries"
    fi
    AC_SUBST(SVGA_CFLAGS)
    AC_SUBST(SVGA_LIBS)
  else
    AC_MSG_RESULT(no)
    no_svga=yes
  fi
else
  AC_MSG_RESULT(disabled)
fi
])

AC_DEFUN([PE_HEADER_VGAKEYBOARD_DEFINE_P],
[
  AC_REQUIRE([PE_PATH_SVGA])
  pe_cppflags="$CPPFLAGS"
  CPPFLAGS="$SVGA_CFLAGS $CPPFLAGS"
  AC_CHECK_HEADER(vgakeyboard.h, [
    AC_CACHE_CHECK(if vgakeyboard.h defines SCANCODE_P,
                   pe_cv_header_vgakeyboard_define_p, [
      AC_EGREP_CPP([scancode_p_defined], [
#include <vgakeyboard.h>
#ifdef SCANCODE_P
int scancode_p_defined;
#endif],
      [pe_cv_header_vgakeyboard_define_p=yes],
      [pe_cv_header_vgakeyboard_define_p=no])
    ])
  ], [no_svga=yes])
  if test "x$pe_cv_header_vgakeyboard_define_p" = xyes; then
    AC_DEFINE([HAVE_DEFINE_SCANCODE_P], [1], [vgakeyboard.h defines SCANCODE_P])
  fi
  CPPFLAGS="$pe_cppflags"
])

AC_DEFUN([PE_HEADER_VGA_HAS_VGA_GETMODENUMBER],
[
  AC_REQUIRE([PE_PATH_SVGA])
  pe_cppflags="$CPPFLAGS"
  CPPFLAGS="$SVGA_CFLAGS $CPPFLAGS"
  pe_libs="$LIBS"
  LIBS="$LIBS $SVGA_LIBS -lvga"
  AC_CACHE_CHECK(if vga.h has vga_getmodenumber,
                 pe_cv_header_vga_has_vga_getmodenumber, [
    AC_TRY_LINK([#include <vga.h>],
    [
      vga_getmodenumber("G320x200x256");
    ],
    [pe_cv_header_vga_has_vga_getmodenumber=yes],
    [pe_cv_header_vga_has_vga_getmodenumber=no])
  ])
  if test "x$pe_cv_header_vga_has_vga_getmodenumber" = xyes; then
    AC_DEFINE([HAVE_VGA_GETMODENUMBER], [1], [svgalib has vga_getmodenumber function])
  fi
  CPPFLAGS="$pe_cppflags"
  LIBS="$pe_libs"
])
