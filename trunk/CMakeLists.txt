

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(LIBAMANITA) 

OPTION(LIBAMANITA_DEBUG "Build the project using debugging code" ON)
IF(LIBAMANITA_DEBUG)
  ADD_DEFINITIONS(-DLIBAMANITA_DEBUG)
ENDIF(LIBAMANITA_DEBUG)

OPTION(DJYNN "Build Djynn-plugin for Geany IDE" ON) 

FIND_PACKAGE(PkgConfig REQUIRED)
FIND_PACKAGE(Threads REQUIRED)


INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
IF(HAVE_UNISTD_H)
  ADD_DEFINITIONS(-DHAVE_UNISTD_H)
ENDIF(HAVE_UNISTD_H)


PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0)
INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK_LIBRARY_DIRS})
ADD_DEFINITIONS(${GTK_CFLAGS_OTHER})

PKG_CHECK_MODULES(WEBKIT REQUIRED webkit-1.0)
INCLUDE_DIRECTORIES(${WEBKIT_INCLUDE_DIRS})
LINK_DIRECTORIES(${WEBKIT_LIBRARY_DIRS})
ADD_DEFINITIONS(${WEBKIT_CFLAGS_OTHER})

PKG_CHECK_MODULES(GEANY geany)
INCLUDE_DIRECTORIES(${GEANY_INCLUDE_DIRS})
LINK_DIRECTORIES(${GEANY_LIBRARY_DIRS})
ADD_DEFINITIONS(${GEANY_CFLAGS_OTHER})

INCLUDE_DIRECTORIES(./include)

IF(UNIX)
  IF(APPLE)
    SET(GUI "Cocoa")
  ELSE(APPLE)
    SET(GUI "GTK+")
  ENDIF(APPLE)
  LINK_LIBRARIES(${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS})
ELSE(UNIX)
  IF(WIN32)
    SET(GUI "Win32")
  ELSE(WIN32)
    SET(GUI "Unknown")
  ENDIF(WIN32)
ENDIF(UNIX)


CONFIGURE_FILE(
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)



MESSAGE("GUI system is ${GUI}")

SET(amanita_src
	src/amanita/aApplication.cpp
	src/amanita/aCollection.cpp
	src/amanita/aDictionary.cpp
	src/amanita/aFile.cpp
	src/amanita/aFlood.cpp
	src/amanita/aHashtable.cpp
	src/amanita/aInteger.cpp
	src/amanita/aMath.cpp
	src/amanita/aObject.cpp
	src/amanita/aPath.cpp
	src/amanita/aRandom.cpp
	src/amanita/aRegex.cpp
	src/amanita/aString.cpp
	src/amanita/aString_HTMLentities.cpp
	src/amanita/aThread.cpp
	src/amanita/aVector.cpp
	src/amanita/aWord.cpp
	src/amanita/config.cpp
	src/amanita/config.h
	src/amanita/gui/aBrowser.cpp
	src/amanita/gui/aSizer.cpp
	src/amanita/gui/aWidget.cpp
	src/amanita/net/aClient.cpp
	src/amanita/net/aHttp.cpp
	src/amanita/net/aServer.cpp
	src/amanita/net/aSocket.cpp
	src/amanita/sqlite/aRecord.cpp
	src/amanita/sqlite/aResultset.cpp
	src/amanita/sqlite/aSQLite.cpp
)

ADD_LIBRARY(amanita STATIC ${amanita_src})


SET(ipomoea_src
	src/ipomoea/hashtable_t.c
	src/ipomoea/string_t.c
	src/ipomoea/vector_t.c
)

ADD_LIBRARY(ipomoea STATIC ${ipomoea_src})


SET(esoteric_src
	src/esoteric/calendar.c
	src/esoteric/config.c
	src/esoteric/astro/planets/earth404.c
	src/esoteric/astro/planets/jupiter404.c
	src/esoteric/astro/planets/mars404.c
	src/esoteric/astro/planets/mercury404.c
	src/esoteric/astro/planets/moon404.c
	src/esoteric/astro/planets/neptune404.c
	src/esoteric/astro/planets/pluto404.c
	src/esoteric/astro/planets/saturn404.c
	src/esoteric/astro/planets/uranus404.c
	src/esoteric/astro/planets/venus404.c
	src/esoteric/astro/astrology.c
	src/esoteric/astro/astronomy.c
	src/esoteric/astro/astronomy_data.c
	src/esoteric/astro/horoscope.c
)

ADD_LIBRARY(esoteric STATIC ${esoteric_src})


if(GEANY_FOUND)
	MESSAGE("Has Geany installed")
	IF(DJYNN)

		SET(djynn_src
			src/plugin/djynn/projectmanager_dlg.c
			src/plugin/djynn/projectmanager.c
			src/plugin/djynn/comment.c
			src/plugin/djynn/sort.c
			src/plugin/djynn/djynn.c
		)
		ADD_LIBRARY(djynn SHARED ${djynn_src})
		SET_TARGET_PROPERTIES(djynn PROPERTIES PREFIX "")

		IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
			MESSAGE("Build Djynn with -fPIC flag")
			ADD_LIBRARY(ipomoea_djynn_pic STATIC ${ipomoea_src})
			SET_TARGET_PROPERTIES(ipomoea_djynn_pic PROPERTIES COMPILE_FLAGS -fpic)
			SET_TARGET_PROPERTIES(djynn PROPERTIES COMPILE_FLAGS -fpic)
			TARGET_LINK_LIBRARIES(djynn ipomoea_djynn_pic ${GTK_LIBRARIES} ${GEANY_LIBRARIES})
		ELSE("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
			MESSAGE("Build Djynn")
			TARGET_LINK_LIBRARIES(djynn ipomoea ${GTK_LIBRARIES} ${GEANY_LIBRARIES})
		ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
	ENDIF(DJYNN)
ENDIF(GEANY_FOUND)



INSTALL(FILES
	include/amanita/aApplication.h
	include/amanita/aBytes.h
	include/amanita/aCollection.h
	include/amanita/aDictionary.h
	include/amanita/aFile.h
	include/amanita/aFlood.h
	include/amanita/aFontmetrics.h
	include/amanita/aHashtable.h
	include/amanita/aInteger.h
	include/amanita/aMath.h
	include/amanita/aObject.h
	include/amanita/aPath.h
	include/amanita/aRandom.h
	include/amanita/aRegex.h
	include/amanita/aString.h
	include/amanita/aThread.h
	include/amanita/aVector.h
	include/amanita/aWord.h
	DESTINATION include/amanita
)

INSTALL(FILES
	include/amanita/gui/aBrowser.h
	include/amanita/gui/aEvent.h
	include/amanita/gui/aSizer.h
	include/amanita/gui/aWidget.h
	DESTINATION include/amanita/gui
)

INSTALL(FILES
	include/amanita/net/aClient.h
	include/amanita/net/aHttp.h
	include/amanita/net/aServer.h
	include/amanita/net/aSocket.h
	DESTINATION include/amanita/net
)

INSTALL(FILES
	include/amanita/sdl/aButton.h
	include/amanita/sdl/aComponent.h
	include/amanita/sdl/aDisplay.h
	include/amanita/sdl/aFont.h
	include/amanita/sdl/aGraphics.h
	include/amanita/sdl/aIconButton.h
	include/amanita/sdl/aImage.h
	include/amanita/sdl/aScroll.h
	include/amanita/sdl/aTabset.h
	include/amanita/sdl/aText.h
	include/amanita/sdl/aTextbox.h
	include/amanita/sdl/aTextfield.h
	include/amanita/sdl/aTooltip.h
	include/amanita/sdl/aWindow.h
	DESTINATION include/amanita/sdl
)

INSTALL(FILES
	include/amanita/sqlite/aRecord.h
	include/amanita/sqlite/aResultset.h
	include/amanita/sqlite/aSQLite.h
	DESTINATION include/amanita/sqlite
)

INSTALL(FILES
	include/ipomoea/hashtable_t.h
	include/ipomoea/string_t.h
	include/ipomoea/vector_t.h
	DESTINATION include/ipomoea
)

INSTALL(FILES
	include/esoteric/calendar.h
	DESTINATION include/esoteric
)

INSTALL(FILES
	include/esoteric/astro/astrology.h
	include/esoteric/astro/astronomy.h
	include/esoteric/astro/horoscope.h
	DESTINATION include/esoteric/astro
)

INSTALL(TARGETS amanita ipomoea esoteric
	DESTINATION lib
)

INSTALL(TARGETS djynn
	DESTINATION /usr/lib/geany
)



ADD_SUBDIRECTORY(examples) 

