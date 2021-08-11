dnl Written by Peter Ekberg, peda@lysator.liu.se

AC_DEFUN([PE_FUNC_PRINTF_RETURN],
[
AC_MSG_CHECKING(return value of printf)
AC_CACHE_VAL(pe_cv_func_printf_return,
  AC_TRY_RUN([
#include <stdio.h>
void main()
{
  FILE *f;
  int return_val;

  f=fopen("conftest.prf", "w");
  if(f == NULL)
    exit(2);
  return_val = fprintf(f, "p");
  fclose(f);
  if(return_val==1)
    exit(0);
  exit(1);
}
    ],
    pe_cv_func_printf_return=POSIX,
    pe_cv_func_printf_return=try_bsd,
    pe_cv_func_printf_return=unknown)
  if test "x$pe_cv_func_printf_return" = xtry_bsd; then
    AC_TRY_RUN([
#include <stdio.h>
void main()
{
  FILE *f;
  int return_val;

  f=fopen("conftest.prf", "w");
  if(f == NULL)
    exit(2);
  return_val = fprintf(f, "p");
  fclose(f);
  exit(!!return_val);
}
        ],
      pe_cv_func_printf_return=BSD,
      pe_cv_func_printf_return=none,
      pe_cv_func_printf_return=unknown)
  fi)

AC_MSG_RESULT($pe_cv_func_printf_return)
case "$pe_cv_func_printf_return" in
  BSD)
    AC_DEFINE([PRINTF_RETURN], [0], [foo]);;
  POSIX)
    AC_DEFINE([PRINTF_RETURN], [1], [foo]);;
  *)
    AC_DEFINE([PRINTF_RETURN], [-1], [foo]);;
esac
])
