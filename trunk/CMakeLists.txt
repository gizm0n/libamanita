
# CMakeLists.txt
#
# To cross build from Linux with MinGW:
# cmake -DCMAKE_TOOLCHAIN_FILE=mingw32.cmake .
#
# This assumes that your MinGW is installed in /usr/i686-w64-mingw32
# and gcc for you crosscompiler is named i686-w64-mingw32-gcc.
# Otherwise copy the cmake-file and replace all instances of i686-w64-mingw32
# with the version of your mingw. Replace [mingw] with the name
# of your mingw:
#
# sed 's/i686-w64-mingw32/[mingw]/g' mingw32.cmake > [mingw].cmake
# cmake -DCMAKE_TOOLCHAIN_FILE=[mingw].cmake .
#


cmake_minimum_required(VERSION 2.8)
project(libamanita) 

message("CMAKE_ROOT: ${CMAKE_ROOT}")
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

include(${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
include(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
if(CMAKE_USE_PTHREADS_INIT)
	set(USE_PTHREADS 1)
else()
	if(CMAKE_USE_WIN32_THREADS_INIT)
		set(USE_WIN32_THREADS 1)
	endif()
endif()

find_package(Gettext REQUIRED)

set(PACKAGE "libamanita")
set(PACKAGE_NAME "Amanita Library")
set(PACKAGE_VERSION_MAJOR 1)
set(PACKAGE_VERSION_MINOR 1)
set(PACKAGE_VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.0")
set(PACKAGE_BUGREPORT "per.lowgren@gmail.com")
set(PACKAGE_URL "http://code.google.com/p/libamanita/")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE}")

option(DJYNN "Build Djynn-plugin for Geany IDE" ON) 
option(ENABLE_NLS "Enable building of translations" ON)

CHECK_INCLUDE_FILE("dlfcn.h" HAVE_DLFCN_H)
CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("memory.h" HAVE_MEMORY_H)
CHECK_INCLUDE_FILE("stdbool.h" HAVE_STDBOOL_H)
CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("strings.h" HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
CHECK_INCLUDE_FILE("sys/stat.h" HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_FUNCTION_EXISTS(realloc HAVE_REALLOC)
CHECK_FUNCTION_EXISTS(malloc HAVE_MALLOC)
CHECK_FUNCTION_EXISTS(floor HAVE_FLOOR)
CHECK_FUNCTION_EXISTS(pow HAVE_POW)
CHECK_FUNCTION_EXISTS(exp HAVE_EXP)
CHECK_FUNCTION_EXISTS(log HAVE_LOG)
CHECK_FUNCTION_EXISTS(sqrt HAVE_SQRT)
CHECK_FUNCTION_EXISTS(memset HAVE_MEMSET)
CHECK_FUNCTION_EXISTS(strchr HAVE_STRCHR)
CHECK_FUNCTION_EXISTS(strdup HAVE_STRDUP)
CHECK_FUNCTION_EXISTS(strnicmp HAVE_STRNICMP)
CHECK_FUNCTION_EXISTS(strpbrk HAVE_STRPBRK)
CHECK_FUNCTION_EXISTS(strstr HAVE_STRSTR)
CHECK_FUNCTION_EXISTS(vprintf HAVE_VPRINTF)

find_package(GTK2 2.6 REQUIRED gtk)
include_directories(${GTK2_INCLUDE_DIRS})
#PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0)
#include_directories(${GTK2_INCLUDE_DIRS})
#link_directories(${GTK2_LIBRARY_DIRS})
#add_definitions(${GTK2_CFLAGS_OTHER})

PKG_CHECK_MODULES(WEBKIT webkit-1.0)
if(WEBKIT_FOUND)
	include_directories(${WEBKIT_INCLUDE_DIRS})
	link_directories(${WEBKIT_LIBRARY_DIRS})
	add_definitions(${WEBKIT_CFLAGS_OTHER})
endif(WEBKIT_FOUND)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_BINARY_DIR}/include"
)

if(CMAKE_TOOLCHAIN_FILE)
endif()

if(UNIX)
	set(USE_UNIX 1)
	if(APPLE)
		message("Apple")
		set(GUI "Cocoa")
		set(GUI_COCOA 1)
		set(USE_COCOA 1)
		set(USE_APPLE 1)
	else()
		message("Unix or Linux")
		set(GUI "GTK+")
		set(GUI_GTK 1)
		set(USE_GTK 1)
		set(USE_GLIB 1)
	endif()
	link_libraries(${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS})
else()
	if(WIN32)
		message("Windows")
		set(GUI "Win32")
		set(UNICODE 1)
		set(USE_WCHAR 1)
		set(GUI_WIN32 1)
		set(USE_WIN32 1)
		link_libraries(intl comctl32 ws2_32)
		add_definitions(-DWIN32_LEAN_AND_MEAN)
		if(MSVC)
			add_definitions(-D_CRT_SECURE_NO_WARNINGS)
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -subsystem:windows")
		endif(MSVC)
#		if(CMAKE_C_COMPILER_ID STREQUAL GNU)
#			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
#		endif()
	else()
		message("Unknown")
		set(GUI "Unknown")
	endif()
endif()


configure_file(
	"${PROJECT_SOURCE_DIR}/include/amanita/_config.h.in"
	"${PROJECT_BINARY_DIR}/include/amanita/_config.h"
)

configure_file(
	"${PROJECT_SOURCE_DIR}/include/amanita/aConfig.h.in"
	"${PROJECT_BINARY_DIR}/include/amanita/aConfig.h"
)



message("GUI system is ${GUI}")

set(amanita_src
	src/amanita/aApplication.cpp
	src/amanita/aCollection.cpp
	src/amanita/aConfig.cpp
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
	src/amanita/gui/aBrowser.cpp
	src/amanita/gui/aButton.cpp
	src/amanita/gui/aCairo.cpp
	src/amanita/gui/aCanvas.cpp
	src/amanita/gui/aChoice.cpp
	src/amanita/gui/aContainer.cpp
	src/amanita/gui/aLabel.cpp
	src/amanita/gui/aList.cpp
	src/amanita/gui/aMenu.cpp
	src/amanita/gui/aNotebook.cpp
	src/amanita/gui/aStatus.cpp
	src/amanita/gui/aText.cpp
	src/amanita/gui/aWidget.cpp
	src/amanita/gui/aWindow.cpp
	src/amanita/net/aClient.cpp
	src/amanita/net/aHttp.cpp
	src/amanita/net/aServer.cpp
	src/amanita/net/aSocket.cpp
	src/amanita/sqlite/aRecord.cpp
	src/amanita/sqlite/aResultset.cpp
	src/amanita/sqlite/aSQLite.cpp
)

add_library(amanita STATIC ${amanita_src})
target_link_libraries(amanita ${CMAKE_THREAD_LIBS_INIT})


set(ipomoea_src
	src/ipomoea/hashtable_t.c
	src/ipomoea/string_t.c
	src/ipomoea/vector_t.c
)

add_library(ipomoea STATIC ${ipomoea_src})


set(esoteric_src
	src/esoteric/calendar.c
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
	src/esoteric/astro/astronomy_fixedstars.c
	src/esoteric/astro/astronomy_orbits.c
	src/esoteric/astro/horoscope.c
)

add_library(esoteric STATIC ${esoteric_src})


install(FILES
	include/amanita/aApplication.h
	include/amanita/aBytes.h
	include/amanita/aCollection.h
	${PROJECT_BINARY_DIR}/include/amanita/aConfig.h
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

install(FILES
	include/amanita/gui/aBrowser.h
	include/amanita/gui/aButton.h
	include/amanita/gui/aCairo.h
	include/amanita/gui/aCanvas.h
	include/amanita/gui/aChoice.h
	include/amanita/gui/aContainer.h
	include/amanita/gui/aEvent.h
	include/amanita/gui/aLabel.h
	include/amanita/gui/aList.h
	include/amanita/gui/aMenu.h
	include/amanita/gui/aNotebook.h
	include/amanita/gui/aStatus.h
	include/amanita/gui/aText.h
	include/amanita/gui/aWidget.h
	include/amanita/gui/aWindow.h
	DESTINATION include/amanita/gui
)

install(FILES
	include/amanita/net/aClient.h
	include/amanita/net/aHttp.h
	include/amanita/net/aServer.h
	include/amanita/net/aSocket.h
	DESTINATION include/amanita/net
)

install(FILES
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

install(FILES
	include/amanita/sqlite/aRecord.h
	include/amanita/sqlite/aResultset.h
	include/amanita/sqlite/aSQLite.h
	DESTINATION include/amanita/sqlite
)

install(FILES
	include/ipomoea/hashtable_t.h
	include/ipomoea/string_t.h
	include/ipomoea/vector_t.h
	DESTINATION include/ipomoea
)

install(FILES
	include/esoteric/calendar.h
	DESTINATION include/esoteric
)

install(FILES
	include/esoteric/astro/astrology.h
	include/esoteric/astro/astronomy.h
	include/esoteric/astro/horoscope.h
	DESTINATION include/esoteric/astro
)

install(TARGETS amanita ipomoea esoteric
	DESTINATION lib
)



PKG_CHECK_MODULES(GEANY geany)
if(GEANY_FOUND)
	message("Has Geany installed")
	include_directories(${GEANY_INCLUDE_DIRS})
	link_directories(${GEANY_LIBRARY_DIRS})
	add_definitions(${GEANY_CFLAGS_OTHER})
	if(DJYNN)

		set(djynn_src
			src/plugin/djynn/projectmanager_dlg.c
			src/plugin/djynn/projectmanager.c
			src/plugin/djynn/comment.c
			src/plugin/djynn/sort.c
			src/plugin/djynn/djynn.c
		)
		add_library(djynn SHARED ${djynn_src})
		set_target_properties(djynn PROPERTIES PREFIX "")

		if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
			message("Build Djynn with -fPIC flag")
			add_library(ipomoea_djynn_pic STATIC ${ipomoea_src})
			set_target_properties(ipomoea_djynn_pic PROPERTIES COMPILE_FLAGS -fpic)
			set_target_properties(djynn PROPERTIES COMPILE_FLAGS -fpic)
			target_link_libraries(djynn ipomoea_djynn_pic ${GTK_LIBRARIES} ${GEANY_LIBRARIES})
		else()
			message("Build Djynn")
			target_link_libraries(djynn ipomoea ${GTK2_LIBRARIES} ${GEANY_LIBRARIES})
		endif()

		install(TARGETS djynn
			DESTINATION /usr/lib/geany
		)
	endif()
endif()




add_subdirectory(examples) 

if(GETTEXT_FOUND AND ENABLE_NLS)
	add_subdirectory(po)
endif()


# CPack installer
include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${PACKAGE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PACKAGE_VERSION_MINOR}")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Per Löwgren")
include(CPack)

