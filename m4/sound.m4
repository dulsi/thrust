dnl Written by Peter Ekberg, peda@lysator.liu.se

AC_DEFUN([PE_LINUX_SOUND],
[
AC_CHECK_HEADERS(linux/soundcard.h, [
  linux_sound=yes
  AC_DEFINE([HAVE_LINUX_SOUND], [1], [Have OSS sound])
  sound=yes
  AC_DEFINE([HAVE_SOUND], [1], [Have sound])
  ])
])


AC_DEFUN([PE_CYGWIN_SOUND],
[
AC_CHECK_HEADERS(sys/soundcard.h, [
  cygwin_sound=yes
  AC_DEFINE([HAVE_CYGWIN_SOUND], [1], [Have Cygwin sound])
  sound=yes
  AC_DEFINE([HAVE_SOUND], [1], [Have sound])
  ])
])


AC_DEFUN([PE_SUN_SOUND],
[
AC_CHECK_HEADERS(sys/audioio.h, sun_sound=yes)
if test "x$sun_sound" != xyes; then
  AC_CHECK_HEADERS(sun/audioio.h, sun_sound=yes)
fi
if test "x$sun_sound" = xyes; then
  AC_MSG_CHECKING(for linear sound encoding)
  AC_CACHE_VAL(pe_cv_sun_linear_sound_encoding,
    AC_EGREP_CPP(Have_audio_encoding_linear, [
#if defined(HAVE_SYS_AUDIOIO_H)
#include <sys/audioio.h>
#elif defined(HAVE_SUN_AUDIOIO_H)
#include <sun/audioio.h>
#endif
#ifdef AUDIO_ENCODING_LINEAR
  Have_audio_encoding_linear
#endif ],
      pe_cv_sun_linear_sound_encoding=yes,
      pe_cv_sun_linear_sound_encoding=no))
  AC_MSG_RESULT($pe_cv_sun_linear_sound_encoding)
  if test "x$pe_cv_sun_linear_sound_encoding" = xyes; then
    AC_DEFINE([HAVE_16BIT_SOUND], [1], [Have 16 bit sound])
    AC_DEFINE([HAVE_SUN_SOUND], [1], [Have SUN sound])
    sound=yes
    AC_DEFINE([HAVE_SOUND], [1], [Have sound])
  else
    sun_sound=
  fi
fi
if test "x$sun_sound" = xyes; then

  AC_MSG_CHECKING(for speaker)
  AC_CACHE_VAL(pe_cv_sun_speaker,
    AC_EGREP_CPP(Have_audio_speaker, [
#if defined(HAVE_SYS_AUDIOIO_H)
#include <sys/audioio.h>
#elif defined(HAVE_SUN_AUDIOIO_H)
#include <sun/audioio.h>
#endif
#ifdef AUDIO_SPEAKER
  Have_audio_speaker
#endif ],
      pe_cv_sun_speaker=yes,
      pe_cv_sun_speaker=no))
  AC_MSG_RESULT($pe_cv_sun_speaker)
  if test "x$pe_cv_sun_speaker" = xyes; then
    AC_DEFINE([HAVE_SUN_SPEAKER], [1], [Have SUN speaker])
  fi

  AC_MSG_CHECKING(for headphone)
  AC_CACHE_VAL(pe_cv_sun_headphone,
    AC_EGREP_CPP(Have_audio_headphone, [
#if defined(HAVE_SYS_AUDIOIO_H)
#include <sys/audioio.h>
#elif defined(HAVE_SUN_AUDIOIO_H)
#include <sun/audioio.h>
#endif
#ifdef AUDIO_HEADPHONE
  Have_audio_headphone
#endif ],
      pe_cv_sun_headphone=yes,
      pe_cv_sun_headphone=no))
  AC_MSG_RESULT($pe_cv_sun_headphone)
  if test "x$pe_cv_sun_headphone" = xyes; then
    AC_DEFINE([HAVE_SUN_HEADPHONE], [1], [Have SUN headphone])
  fi

  AC_MSG_CHECKING(for line out)
  AC_CACHE_VAL(pe_cv_sun_line_out,
    AC_EGREP_CPP(Have_audio_line_out, [
#if defined(HAVE_SYS_AUDIOIO_H)
#include <sys/audioio.h>
#elif defined(HAVE_SUN_AUDIOIO_H)
#include <sun/audioio.h>
#endif
#ifdef AUDIO_LINE_OUT
  Have_audio_line_out
#endif ],
      pe_cv_sun_line_out=yes,
      pe_cv_sun_line_out=no))
  AC_MSG_RESULT($pe_cv_sun_line_out)
  if test "x$pe_cv_sun_line_out" = xyes; then
    AC_DEFINE([HAVE_SUN_LINE_OUT], [1], [Have SUN line out])
  fi
fi
])


AC_DEFUN([PE_HP_SOUND],
[
AC_CHECK_HEADERS(sys/audio.h, hp_sound=yes)
if test "x$hp_sound" = xyes; then
  AC_MSG_CHECKING(for linear sound encoding)
  AC_CACHE_VAL(pe_cv_hp_linear_sound_encoding,
    AC_EGREP_CPP(Have_audio_encoding_linear, [
#include <sys/audio.h>
#ifdef AUDIO_FORMAT_LINEAR16BIT
  Have_audio_encoding_linear
#endif ],
      pe_cv_hp_linear_sound_encoding=yes,
      pe_cv_hp_linear_sound_encoding=no))
  AC_MSG_RESULT($pe_cv_hp_linear_sound_encoding)
  if test "x$pe_cv_hp_linear_sound_encoding" = xyes; then
    AC_DEFINE([HAVE_16BIT_SOUND], [1], [Have 16 bit sound])
    AC_DEFINE([HAVE_HP_SOUND], [1], [Have HP sound])
    sound=yes
    AC_DEFINE([HAVE_SOUND], [1], [Have sound])
  else
    hp_sound=
  fi
fi
if test "x$hp_sound" = xyes; then

  AC_MSG_CHECKING(for internal_speaker)
  AC_CACHE_VAL(pe_cv_hp_internal_speaker,
    AC_EGREP_CPP(Have_audio_internal_speaker, [
#include <sys/audio.h>
#ifdef AUDIO_OUT_INTERNAL
  Have_audio_internal_speaker
#endif ],
      pe_cv_hp_internal_speaker=yes,
      pe_cv_hp_internal_speaker=no))
  AC_MSG_RESULT($pe_cv_hp_internal_speaker)
  if test "x$pe_cv_hp_internal_speaker" = xyes; then
    AC_DEFINE([HAVE_HP_INTERNAL_SPEAKER], [1], [Have HP internal speaker])
  fi

  AC_MSG_CHECKING(for external speaker)
  AC_CACHE_VAL(pe_cv_hp_external_speaker,
    AC_EGREP_CPP(Have_audio_external_speaker, [
#include <sys/audio.h>
#ifdef AUDIO_OUT_EXTERNAL
  Have_audio_external_speaker
#endif ],
      pe_cv_hp_external_speaker=yes,
      pe_cv_hp_external_speaker=no))
  AC_MSG_RESULT($pe_cv_hp_external_speaker)
  if test "x$pe_cv_hp_external_speaker" = xyes; then
    AC_DEFINE([HAVE_HP_EXTERNAL_SPEAKER], [1], [Have HP external speaker])
  fi

  AC_MSG_CHECKING(for line out)
  AC_CACHE_VAL(pe_cv_hp_line_out,
    AC_EGREP_CPP(Have_audio_line_out, [
#include <sys/audio.h>
#ifdef AUDIO_OUT_LINE
  Have_audio_line_out
#endif ],
      pe_cv_hp_line_out=yes,
      pe_cv_hp_line_out=no))
  AC_MSG_RESULT($pe_cv_hp_line_out)
  if test "x$pe_cv_hp_line_out" = xyes; then
    AC_DEFINE([HAVE_HP_LINE_OUT], [1], [Have HP line out])
  fi
fi
])
