Project (thrust)
CMake_Minimum_Required (VERSION 3.1)

Set (BINDIR "bin" CACHE STRING "Where to install binaries")

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)

add_definitions(-DHAVE_CONFIG_H)

#Add the include directories of the (found) libraries.
Include_Directories(
 	${SDL2_INCLUDE_DIRS}
 	${SDL2_MIXER_INCLUDE_DIRS}
)

file(GLOB SOURCES   src/thrust.c src/fast_gr.c src/hiscore.c src/conf.c src/things.c
  src/init.c src/level.c src/font5x5.c src/graphics.c
  datasrc/blks.c datasrc/ship.c datasrc/shld.c datasrc/colors.c
  datasrc/bullet.c datasrc/title.c datasrc/demomove.c
  datasrc/level1.c datasrc/level2.c datasrc/level3.c
  datasrc/level4.c datasrc/level5.c datasrc/level6.c
  datasrc/font.c
  datasrc/blip.c datasrc/boom.c datasrc/boom2.c datasrc/harp.c
  datasrc/engine.c datasrc/zero.c
  src/sdl.c src/sdlsound.c src/sdlkey.c src/statistics.c
)

Add_Executable (sdlthrust ${SOURCES})
Target_Link_Libraries (
 	sdlthrust
 	${SDL2_LIBRARIES}
 	${SDL2_MIXER_LIBRARIES}
  -lm
)

Install (TARGETS sdlthrust RUNTIME DESTINATION ${BINDIR})
