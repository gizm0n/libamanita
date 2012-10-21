
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

option(EXAMPLES "Build Amanita Library examples" ON) 
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
if(GTK2_FOUND)
	include_directories(${GTK2_INCLUDE_DIRS})
	add_definitions(-DGTK)
endif()
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

PKG_CHECK_MODULES(SOURCEVIEW gtksourceview-2.0)
if(SOURCEVIEW_FOUND)
	include_directories(${SOURCEVIEW_INCLUDE_DIRS})
	link_directories(${SOURCEVIEW_LIBRARY_DIRS})
	add_definitions(${SOURCEVIEW_CFLAGS_OTHER})
	set(HAVE_SOURCEVIEW 1)
endif(SOURCEVIEW_FOUND)

CHECK_INCLUDE_FILE("Scintilla.h" HAVE_SCINTILLA)
if(HAVE_SCINTILLA)
	if(WIN32)
	else()
		set(SCINTILLA_LIB scintilla)
	endif()
endif(HAVE_SCINTILLA)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_BINARY_DIR}/include"
)

if(CMAKE_TOOLCHAIN_FILE)
endif()

if(CMAKE_C_COMPILER_ID STREQUAL GNU)
	SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
	SET(CMAKE_CXX_FLAGS_RELEASE "-O1")
	SET(CMAKE_CXX_FLAGS_DEBUG  "-Werror -O0 -g")
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

		set(PACKAGE_DATADIR ${CMAKE_INSTALL_PREFIX}/share/amanita)
		set(PACKAGE_ICONSDIR ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor)
		set(PACKAGE_LOCALEDIR ${CMAKE_INSTALL_PREFIX}/share/locale)
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
		link_libraries(intl comctl32 ws2_32 ole32)
		add_definitions(-DWIN32_LEAN_AND_MEAN)

		set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -O coff -o <OBJECT> <SOURCE>")
		enable_language(RC)

		if(MSVC)
			add_definitions(-D_CRT_SECURE_NO_WARNINGS)
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -subsystem:windows")
		endif(MSVC)
#		if(CMAKE_C_COMPILER_ID STREQUAL GNU)
#			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
#		endif()
		if(MINGW)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
			set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
			set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -static-libgcc -static-libstdc++ -s")
		endif()

		set(PACKAGE_DATADIR data)
		set(PACKAGE_ICONSDIR icons)
		set(PACKAGE_LOCALEDIR "data\\locale")
	else()
		message("Unknown")
		set(GUI "Unknown")
	endif()
endif()


if(UNIX)
	if(APPLE)
	else()
		install(FILES icons/16x16/amanita.xpm DESTINATION share/pixmaps)
		install(FILES icons/16x16/amanita.png DESTINATION share/icons/hicolor/16x16/apps)
		install(FILES icons/48x48/amanita.png DESTINATION share/icons/hicolor/48x48/apps)

		set(AMANITA_APP_ICON ${PACKAGE_ICONSDIR}/16x16/amanita.png)
	endif()
else()
	if(MINGW)
		install(FILES
			icons/amanita.ico
			icons/stock_icons.bmp
			DESTINATION ${PACKAGE_ICONSDIR})
		set(AMANITA_APP_ICON "${CMAKE_INSTALL_PREFIX}/${PACKAGE_ICONSDIR}/amanita.ico")
		set(AMANITA_STOCK_ICONS "${CMAKE_INSTALL_PREFIX}/${PACKAGE_ICONSDIR}/stock_icons.bmp")
	endif()
endif()


configure_file(
	"${PROJECT_SOURCE_DIR}/include/amanita/_config.h.in"
	"${PROJECT_BINARY_DIR}/include/amanita/_config.h"
)

configure_file(
	"${PROJECT_SOURCE_DIR}/include/amanita/Config.h.in"
	"${PROJECT_BINARY_DIR}/include/amanita/Config.h"
)

configure_file(
	"${PROJECT_SOURCE_DIR}/include/amanita/Resource.h.in"
	"${PROJECT_BINARY_DIR}/include/amanita/Resource.h"
)



message("GUI system is ${GUI}")

set(amanita_src
	src/amanita/Application.cpp
	src/amanita/Collection.cpp
	src/amanita/Config.cpp
	src/amanita/Dictionary.cpp
	src/amanita/File.cpp
	src/amanita/Flood.cpp
	src/amanita/Hashtable.cpp
	src/amanita/Integer.cpp
	src/amanita/Math.cpp
	src/amanita/Object.cpp
	src/amanita/Path.cpp
	src/amanita/Properties.cpp
	src/amanita/Random.cpp
	src/amanita/Regex.cpp
	src/amanita/String.cpp
	src/amanita/Thread.cpp
	src/amanita/Vector.cpp
	src/amanita/Wiki.cpp
	src/amanita/Word.cpp
	src/amanita/db/Database.cpp
	src/amanita/db/Record.cpp
	src/amanita/db/Result.cpp
	src/amanita/gui/Button.cpp
	src/amanita/gui/Display.cpp
	src/amanita/gui/Font.cpp
	src/amanita/gui/Graphics.cpp
	src/amanita/gui/Icon.cpp
	src/amanita/gui/Image.cpp
	src/amanita/gui/Notebook.cpp
	src/amanita/gui/Scroll.cpp
	src/amanita/gui/Text.cpp
	src/amanita/gui/Textbox.cpp
	src/amanita/gui/Textfield.cpp
	src/amanita/gui/Tooltip.cpp
	src/amanita/gui/Widget.cpp
	src/amanita/gui/Window.cpp
	src/amanita/net/Client.cpp
	src/amanita/net/Http.cpp
	src/amanita/net/Server.cpp
	src/amanita/net/Socket.cpp
	src/amanita/tk/Browser.cpp
	src/amanita/tk/Button.cpp
	src/amanita/tk/Cairo.cpp
	src/amanita/tk/Canvas.cpp
	src/amanita/tk/Container.cpp
	src/amanita/tk/Label.cpp
	src/amanita/tk/List.cpp
	src/amanita/tk/Menu.cpp
	src/amanita/tk/Notebook.cpp
	src/amanita/tk/Panel.cpp
	src/amanita/tk/Select.cpp
	src/amanita/tk/Status.cpp
	src/amanita/tk/Text.cpp
	src/amanita/tk/Widget.cpp
	src/amanita/tk/Window.cpp
)

add_library(amanita STATIC ${amanita_src})
target_link_libraries(amanita ${CMAKE_THREAD_LIBS_INIT})


set(ipomoea_src
	src/ipomoea/base64.c
	src/ipomoea/hashtable_t.c
	src/ipomoea/rle.c
	src/ipomoea/string_t.c
	src/ipomoea/vector_t.c
)

add_library(ipomoea STATIC ${ipomoea_src})


set(mandragora_src
	src/mandragora/calendar.c
	src/mandragora/astro/planets/earth404.c
	src/mandragora/astro/planets/jupiter404.c
	src/mandragora/astro/planets/mars404.c
	src/mandragora/astro/planets/mercury404.c
	src/mandragora/astro/planets/moon404.c
	src/mandragora/astro/planets/neptune404.c
	src/mandragora/astro/planets/pluto404.c
	src/mandragora/astro/planets/saturn404.c
	src/mandragora/astro/planets/uranus404.c
	src/mandragora/astro/planets/venus404.c
	src/mandragora/astro/astrology.c
	src/mandragora/astro/astronomy.c
	src/mandragora/astro/astronomy_data.c
	src/mandragora/astro/astronomy_fixedstars.c
	src/mandragora/astro/astronomy_orbits.c
	src/mandragora/astro/horoscope.c
)

add_library(mandragora STATIC ${mandragora_src})


install(FILES
	include/amanita/Application.h
	include/amanita/Bytes.h
	include/amanita/Collection.h
	${PROJECT_BINARY_DIR}/include/amanita/Config.h
	include/amanita/Dictionary.h
	include/amanita/File.h
	include/amanita/Flood.h
	include/amanita/Fontmetrics.h
	include/amanita/Hashtable.h
	include/amanita/Integer.h
	include/amanita/Math.h
	include/amanita/Object.h
	include/amanita/Path.h
	include/amanita/Properties.h
	include/amanita/Random.h
	include/amanita/Regex.h
	${PROJECT_BINARY_DIR}/include/amanita/Resource.h
	include/amanita/String.h
	include/amanita/Thread.h
	include/amanita/Vector.h
	include/amanita/Wiki.h
	include/amanita/Word.h
	DESTINATION include/amanita
)

install(FILES
	include/amanita/db/Database.h
	include/amanita/db/Record.h
	include/amanita/db/Result.h
	DESTINATION include/amanita/db
)

install(FILES
	include/amanita/gui/Button.h
	include/amanita/gui/Display.h
	include/amanita/gui/Event.h
	include/amanita/gui/Font.h
	include/amanita/gui/Graphics.h
	include/amanita/gui/Icon.h
	include/amanita/gui/Image.h
	include/amanita/gui/Notebook.h
	include/amanita/gui/Scroll.h
	include/amanita/gui/Text.h
	include/amanita/gui/Textbox.h
	include/amanita/gui/Textfield.h
	include/amanita/gui/Tooltip.h
	include/amanita/gui/Widget.h
	include/amanita/gui/Window.h
	DESTINATION include/amanita/gui
)

install(FILES
	include/amanita/net/Client.h
	include/amanita/net/Http.h
	include/amanita/net/Server.h
	include/amanita/net/Socket.h
	DESTINATION include/amanita/net
)

install(FILES
	include/amanita/tk/Browser.h
	include/amanita/tk/Button.h
	include/amanita/tk/Cairo.h
	include/amanita/tk/Canvas.h
	include/amanita/tk/Container.h
	include/amanita/tk/Label.h
	include/amanita/tk/List.h
	include/amanita/tk/Menu.h
	include/amanita/tk/Notebook.h
	include/amanita/tk/Panel.h
	include/amanita/tk/Select.h
	include/amanita/tk/Status.h
	include/amanita/tk/Text.h
	include/amanita/tk/Widget.h
	include/amanita/tk/Window.h
	DESTINATION include/amanita/tk
)

install(FILES
	include/ipomoea/base64.h
	include/ipomoea/hashtable_t.h
	include/ipomoea/rle.h
	include/ipomoea/string_t.h
	include/ipomoea/vector_t.h
	DESTINATION include/ipomoea
)

install(FILES
	include/mandragora/calendar.h
	DESTINATION include/mandragora
)

install(FILES
	include/mandragora/astro/astrology.h
	include/mandragora/astro/astronomy.h
	include/mandragora/astro/horoscope.h
	DESTINATION include/mandragora/astro
)

install(TARGETS
	amanita
	ipomoea
	mandragora
	DESTINATION lib
)



if(EXAMPLES)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/examples)

	if(WIN32)
		if(MINGW)
			# resource compilation for mingw
			add_custom_command(
				OUTPUT ${PROJECT_BINARY_DIR}/CMakeFiles/examples_resource.obj
				COMMAND ${CMAKE_RC_COMPILER} -I${PROJECT_SOURCE_DIR}/examples
				-I${PROJECT_SOURCE_DIR}/include -I${PROJECT_BINARY_DIR}/include
				-i${PROJECT_SOURCE_DIR}/examples/_resource.rc
				-o ${PROJECT_BINARY_DIR}/CMakeFiles/examples_resource.obj)
			set(RC_FILE ${PROJECT_BINARY_DIR}/CMakeFiles/examples_resource.obj)
		else()
			set(RC_FILE examples/_resource.rc)
		endif()
	endif()

	add_executable(babble examples/babble.cpp)
	target_link_libraries(babble amanita)

	if(WIN32)
		add_executable(browser WIN32 examples/browser.cpp ${RC_FILE})
	else()
		add_executable(browser examples/browser.cpp)
	endif()
	target_link_libraries(browser amanita ${GTK2_LIBRARIES} ${WEBKIT_LIBRARIES})

	add_executable(client examples/client.cpp)
	target_link_libraries(client amanita ${CMAKE_THREAD_LIBS_INIT} ${GTK2_LIBRARIES})

	add_executable(database examples/database.cpp)
	target_link_libraries(database amanita sqlite3)

	add_executable(entropy examples/entropy.cpp)
	target_link_libraries(entropy amanita)

	if(WIN32)
		add_executable(gui WIN32 examples/gui.cpp ${RC_FILE})
	else()
		add_executable(gui examples/gui.cpp)
	endif()
	target_link_libraries(gui amanita ${GTK2_LIBRARIES} ${WEBKIT_LIBRARIES} ${SCINTILLA_LIB} ${SOURCEVIEW_LIBRARIES})

	add_executable(hashtable examples/hashtable.cpp)
	target_link_libraries(hashtable amanita)

	add_executable(horoscope examples/horoscope.c)
	target_link_libraries(horoscope mandragora m)

	add_executable(http examples/http.cpp)
	target_link_libraries(http amanita ${CMAKE_THREAD_LIBS_INIT} ${GTK2_LIBRARIES})

	add_executable(properties examples/properties.cpp)
	target_link_libraries(properties amanita)
	ADD_CUSTOM_COMMAND(TARGET properties
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/examples/properties.cfg ${PROJECT_BINARY_DIR}/examples/properties.cfg
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/examples/properties.ini ${PROJECT_BINARY_DIR}/examples/properties.ini
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/examples/properties.properties ${PROJECT_BINARY_DIR}/examples/properties.properties
	)

	add_executable(regex examples/regex.cpp)
	target_link_libraries(regex amanita)

	add_executable(server examples/server.cpp)
	target_link_libraries(server amanita ${CMAKE_THREAD_LIBS_INIT} ${GTK2_LIBRARIES})

	add_executable(string examples/string.cpp)
	target_link_libraries(string amanita)

	add_executable(wiki examples/wiki.cpp)
	target_link_libraries(wiki amanita)
endif()


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

