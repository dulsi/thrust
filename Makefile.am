## Process this file with automake to produce Makefile.in

# Written by Peter Ekberg, peda@lysator.liu.se

AM_CPPFLAGS = @GGI_CFLAGS@ @X_CFLAGS@ @SVGA_CFLAGS@ @DIRECTX_CFLAGS@

BIN8         = \
  datasrc/blks0.bin datasrc/blks1.bin datasrc/blks2.bin datasrc/blks3.bin \
  datasrc/blks4.bin datasrc/blks5.bin datasrc/blks6.bin datasrc/blks7.bin \
  datasrc/blks8.bin datasrc/blks9.bin datasrc/blksa.bin datasrc/blksb.bin \
  datasrc/blksc.bin datasrc/blksd.bin datasrc/blkse.bin datasrc/blksf.bin

bin_PROGRAMS =

SRC = \
  src/thrust.c src/fast_gr.c src/hiscore.c src/conf.c src/things.c \
  src/init.c src/level.c src/font5x5.c src/graphics.c src/compat.h \
  src/conf.h src/fast_gr.h src/font5x5.h src/gr_drv.h src/graphics.h \
  src/hiscore.h src/init.h src/keyboard.h src/level.h src/options.h \
  src/sound.h src/soundIt.h src/things.h src/thrust.h src/thrust_t.h

GENDATASRC = \
  datasrc/blks.c datasrc/ship.c datasrc/shld.c datasrc/colors.c \
  datasrc/bullet.c datasrc/title.c datasrc/demomove.c \
  datasrc/level1.c datasrc/level2.c datasrc/level3.c \
  datasrc/level4.c datasrc/level5.c datasrc/level6.c

DATASRC = $(GENDATASRC) datasrc/font.c

SOUNDS = \
  datasrc/blip.c datasrc/boom.c datasrc/boom2.c datasrc/harp.c \
  datasrc/engine.c datasrc/zero.c

LIBS = -lm

if GGI_BACKEND
bin_PROGRAMS += thrust
thrust_SOURCES = $(SRC) \
	src/ggi.c src/ggikey.c src/giik2str.c \
	$(DATASRC)
thrust_LDADD = @GGI_LIBS@ @GGI_EXTRA_LIBS@ -lggi -lgg
if COMPAT
thrust_LDADD += lib/libcompat.a
endif
if DSOUND
thrust_SOURCES += src/DirectSound.c $(SOUNDS)
thrust_LDADD   += @DIRECTX_LIBS@ -ldsound
else
if SOUNDIT
thrust_SOURCES += src/soundIt.c $(SOUNDS)
else
thrust_SOURCES += src/silence.c
endif
endif
endif

if X_BACKEND
bin_PROGRAMS += xthrust
xthrust_SOURCES = $(SRC) $(DATASRC) \
	src/X11key.c src/X11.c src/X11.h datasrc/icon48.c
xthrust_LDADD = @X_PRE_LIBS@ @X_LIBS@ @X_EXTRA_LIBS@ -lXext -lX11
if COMPAT
xthrust_LDADD += lib/libcompat.a
endif
if SOUNDIT
xthrust_SOURCES += src/soundIt.c $(SOUNDS)
else
xthrust_SOURCES += src/silence.c
endif
endif

if DIRECTX_BACKEND
bin_PROGRAMS += dxthrust
dxthrust_SOURCES = $(SRC) $(DATASRC) \
	src/DirectX.c src/win32key.c src/resource.h
dxthrust_LDADD = thrust_rc.$(OBJEXT) @DIRECTX_LIBS@ -lddraw -lgdi32
if COMPAT
dxthrust_LDADD += lib/libcompat.a
endif
if DSOUND
dxthrust_SOURCES += src/DirectSound.c $(SOUNDS)
dxthrust_LDADD   += -ldsound
else
dxthrust_SOURCES += src/silence.c
endif

thrust_rc.o: datasrc/thrust_rc.rc datasrc/nocursor.cur datasrc/thrust.ico
	$(RC) $< $@

thrust_rc.obj: datasrc/thrust_rc.rc datasrc/nocursor.cur datasrc/thrust.ico
	$(RC) $< $@

endif

if SVGA_BACKEND
bin_PROGRAMS += svgathrust
svgathrust_SOURCES = $(SRC) $(DATASRC) \
	src/svgakey.c src/ksyms.c src/ksyms.h src/svga.c
svgathrust_LDADD = @SVGA_LIBS@ -lvgagl -lvga
if COMPAT
svgathrust_LDADD += lib/libcompat.a
endif
if SOUNDIT
svgathrust_SOURCES += src/soundIt.c $(SOUNDS)
else
svgathrust_SOURCES += src/silence.c
endif
endif

man6_MANS = thrust.man

highscoredir = $(sharedstatedir)
highscore_DATA = thrust.highscore

LEGACY = \
  src/doskey.c src/doskey.h \
  helpers/bin2c.c helpers/reverse.c helpers/skip.c helpers/txt2c.c

EXTRA_DIST = \
  CHANGES colors.txt thrust-@PACKAGE_VERSION@.lsm \
  thrust.highscore thrustrc thrust.pod thrust.man \
  datasrc/blip.snd datasrc/boom.snd datasrc/boom2.snd \
  datasrc/harp.snd datasrc/engine.snd datasrc/zero.snd \
  datasrc/blks0-8.bmp datasrc/blks1-8.bmp datasrc/blks2-8.bmp datasrc/blks3-8.bmp \
  datasrc/blks4-8.bmp datasrc/blks5-8.bmp datasrc/blks6-8.bmp datasrc/blks7-8.bmp \
  datasrc/blks8-8.bmp datasrc/blks9-8.bmp datasrc/blksa-8.bmp datasrc/blksb-8.bmp \
  datasrc/blksc-8.bmp datasrc/blksd-8.bmp datasrc/blkse-8.bmp datasrc/blksf-8.bmp \
  datasrc/bullet-4.bmp datasrc/ship-16.bmp datasrc/shld-17.bmp \
  datasrc/colors.pal datasrc/demomove.bin datasrc/icon48.ppm datasrc/title.ppm \
  datasrc/nocursor.cur datasrc/thrust.ico \
  datasrc/level1.def datasrc/level2.def datasrc/level3.def \
  datasrc/level4.def datasrc/level5.def datasrc/level6.def \
  $(LEGACY)

SUBDIRS =

if COMPAT
SUBDIRS += lib
endif

if MAINTAINER_MODE

# Argh, GNU make specific...
.INTERMEDIATE: $(BIN8) $(srcdir)/datasrc/blks.bin

DISTCLEANFILES = thrust.html
MAINTAINERCLEANFILES = \
	$(GENDATASRC) $(SOUNDS) datasrc/icon48.c $(BIN8) datasrc/blks.bin \
	thrust.man configure Makefile.in aclocal.m4 src/config.h.in

SUBDIRS += helpers

ACLOCAL_AMFLAGS = -I m4

SUFFIXES = .snd .def .bin .ppm -4.rev -8.rev -16.rev -17.rev .rev .bmp .pal

$(srcdir)/datasrc/blks.bin: $(BIN8)
	cat $(BIN8) > $@

# Extract palette information.
#%.bin: %.pal helpers/skip$(EXE)
#	helpers/skip 790 < $< > $@
.pal.bin:
	AWK=$(AWK) $(srcdir)/helpers/skip 790 < $< > $@
# Extract picture information.
#%.rev: %.bmp helpers/skip$(EXE)
#	helpers/skip 1078 < $< > $@
.bmp.rev:
	AWK=$(AWK) $(srcdir)/helpers/skip 1078 < $< > $@


# The bmp format stores rows backwards (last row first).
# These rules are used to "reverse" the order in which the rows are stored.
# There is one rule for each of the four used widths of the line.
-4.rev.bin:
	AWK=$(AWK) $(srcdir)/helpers/reverse 4 < $< > $@
-8.rev.bin:
	AWK=$(AWK) $(srcdir)/helpers/reverse 8 < $< > $@
-16.rev.bin:
	AWK=$(AWK) $(srcdir)/helpers/reverse 16 < $< > $@
-17.rev.bin:
	AWK=$(AWK) $(srcdir)/helpers/reverse 20 3 < $< > $@

# Make a C file out of a text file. The info is stored in an array of strings.
# One line in the original text file will be stored in one cell of the array.
.def.c:
	AWK=$(AWK) $(srcdir)/helpers/txt2c `echo "$<" | \
	  sed -e 's/.*[/\\]//' -e 's/\.[^.]*$$//'` < $< > $@

# Make a C file out of a binary file. The info is stored in a byte array.
.bin.c:
	AWK=$(AWK) $(srcdir)/helpers/bin2c bin_`echo "$<" | \
	  sed -e 's/.*[/\\]//' -e 's/\.[^.]*$$//'` < $< > $@

# Make a C file out of a sound file.
.snd.c:
	printf "\nunsigned int sound_%s_len = %d;\n" \
	  `echo "$<" | sed -e 's/.*[/\\]//' -e 's/\.[^.]*$$//'` \
	  `wc -c < $<` > $@
	AWK=$(AWK) $(srcdir)/helpers/bin2c sound_`echo "$<" | \
	  sed -e 's/.*[/\\]//' -e 's/\.[^.]*$$//'` < $< >> $@

# Make a C file out of a ppm picture file.
#ifeq ($(NO_PBM),yes)
#%.c: %.ppm
#	@echo Must configure with pbm to build ppm2c.
#	@echo Warning: Unable to rebuild $@ from $<.
#else
#%.c: %.ppm helpers/ppm2c$(EXE)
#	helpers/ppm2c -n $(notdir $(basename $<)) -m 32 < $< > $@
#endif
.ppm.c:
	helpers/ppm2c -n `echo "$<" | \
	  sed -e 's/.*[/\\]//' -e 's/\.[^.]*$$//'` -m 32 < $< > $@


# Make a man page from a pod source
#%.man: %.pod
#ifeq ($(POD2MAN),)
#	@echo Must have pod2man to rebuild man page.
#	@echo Warning: Unable to rebuild $@ from $<.
#else
.pod.man:
	@POD2MAN@ $< > $@
#endif

# Make html man page from a pod source
#%.html: %.pod
#ifeq ($(POD2MAN),)
#	@echo Must have pod2html to rebuild html docs.
#	@echo Warning: Unable to rebuild $@ from $<.
#else
.pod.html:
	@POD2HTML@ $< > $@
	rm -f pod2htmd.x~~ pod2htmi.x~~ pod2htmd.tmp pod2htmi.tmp
#endif

endif
