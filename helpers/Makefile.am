## Process this file with automake to produce Makefile.in

# Written by Peter Ekberg, peda@lysator.liu.se

if MAINTAINER_MODE
noinst_PROGRAMS = ppm2c
endif

AM_CPPFLAGS = @PBM_CFLAGS@ -DPPM2C_VERSION=\"0.6\"
LDADD = @PBM_LIBS@

if COMPAT
LDADD += ../lib/libcompat.a
endif

EXTRA_DIST = bin2c reverse skip txt2c
