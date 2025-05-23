Project (inertiablast C)
CMake_Minimum_Required (VERSION 3.1)

Set (BINDIR "bin" CACHE STRING "Where to install binaries")
Set (DATAROOTDIR "${CMAKE_INSTALL_PREFIX}/share" CACHE STRING "Sets the root of data directories to a non-default location")
Set (ICONDIR "${DATAROOTDIR}/icons" CACHE STRING "Sets the icon directory for desktop entry to a non-default location.")

add_definitions(-DDATA_DIR=\"${DATAROOTDIR}/inertiablast\")

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)

# Find LibGamerzilla library
if (NOT APPLE)
    pkg_search_module(GAMERZILLA OPTIONAL gamerzilla)

    if (GAMERZILLA_LIBRARIES)
        message(STATUS "Gamerzilla found")
        add_definitions(-DHAVE_GAMERZILLA)
    endif()
endif()

add_definitions(-DHAVE_CONFIG_H)

#Add the include directories of the (found) libraries.
Include_Directories(
 	${SDL2_INCLUDE_DIRS}
 	${SDL2_MIXER_INCLUDE_DIRS}
  ${GAMERZILLA_INCLUDE_DIRS}
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

Add_Executable (inertiablast ${SOURCES})
Target_Link_Libraries (
 	inertiablast
 	${SDL2_LIBRARIES}
 	${SDL2_MIXER_LIBRARIES}
  ${GAMERZILLA_LIBRARIES}
  -lm
)

Install (DIRECTORY ${PROJECT_SOURCE_DIR}/gamerzilla DESTINATION ${DATAROOTDIR}/inertiablast/)
Install (TARGETS inertiablast RUNTIME DESTINATION ${BINDIR})

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	Install (FILES datasrc/icon.png DESTINATION ${ICONDIR}/hicolor/48x48/apps/ RENAME inertiablast.png)
	Install (FILES inertiablast.desktop DESTINATION ${DATAROOTDIR}/applications/)
	Install (FILES inertiablast.metainfo.xml DESTINATION ${DATAROOTDIR}/metainfo/)
	Install (FILES inertiablast.man DESTINATION ${DATAROOTDIR}/man/man6/ RENAME inertiablast.6)
endif ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
