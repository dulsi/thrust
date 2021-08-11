dnl Written by Peter Ekberg, peda@lysator.liu.se

AC_DEFUN([PE_VALUES_H_DEFINE_MAXINT],
[
  AC_CHECK_HEADER(values.h, [
    AC_CACHE_CHECK(if values.h defines MAXINT,
                   pe_cv_header_values_define_maxint, [
      AC_EGREP_CPP([maxint_defined], [
#include <values.h>
#ifdef MAXINT
int maxint_defined;
#endif],
      [pe_cv_header_values_define_maxint=yes],
      [pe_cv_header_values_define_maxint=no])
    ])
  ])
  if test "x$pe_cv_header_values_define_maxint" = xyes; then
    AC_DEFINE([HAVE_VALUES_H], [1], [values.h defines MAXINT])
  fi
])
