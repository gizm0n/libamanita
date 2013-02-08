
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
# > sed 's/i686-w64-mingw32/[mingw]/g' mingw32.cmake > [mingw].cmake
# > cmake -DCMAKE_TOOLCHAIN_FILE=[mingw].cmake .
#
# You will have to update prefix for pkgconfig files, run this script:
# > data/pkg-config-fix.pl /usr/i686-w64-mingw32
#


cmake_minimum_required(VERSION 2.8)
project(libamanita)

if(UNIX AND NOT APPLE)
	set(LINUX 1)
endif()

set(PACKAGE "libamanita")
set(PACKAGE_NAME "Amanita Library")
set(PACKAGE_VERSION_MAJOR 1)
set(PACKAGE_VERSION_MINOR 1)
set(PACKAGE_VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.0")
set(PACKAGE_BUGREPORT "per.lowgren@gmail.com")
set(PACKAGE_URL "http://code.google.com/p/libamanita/")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE}")

string(ASCII 27 _escape)
set(CYAN "${_escape}[0;36m")
set(BLUE "${_escape}[0;34m")
set(RED "${_escape}[1;31m")
set(GREEN "${_escape}[0;32m")
set(BROWN "${_escape}[0;33m")
set(YELLOW "${_escape}[1;33m")
set(NONE "${_escape}[0m")

message("\n${CYAN}**************************************-INSTALLING-PACKAGES-*************************************${NONE}")
if(LINUX)
	message(
		"${GREEN}Building ${PACKAGE_NAME} on Linux.\n"
		"\n"
		"All packages are in the repositories. For audio and video functionality you will have to add\n"
		"external repositories such as Medibuntu for Ubuntu, begacuse of legal reasons.\n"
		"\n"
		"Uninstalled packages give you a message in red telling you which package(s) to install.")
elseif(MINGW)
	message(
		"${GREEN}Building ${PACKAGE_NAME} using MingW.\n"
		"\n"
		"Look for RPM-packages in the Fedora repositories. All packages starting\n"
		"with 'mingw32' works.\n"
		"\n"
		"Latest version of FFMPEG is here (you need both shared and devel):\n"
		"${BLUE}http://ffmpeg.zeranoe.com/builds/win32/${GREEN}\n"
		"\n"
		"Many packages can be found as DevPak at: ${BLUE}http://devpaks.org${GREEN}\n"
		"To extract a DevPak, rename the file from [name].Devpak to [name].tar.bz2 and extract as usual.\n"
		"The only package you need at the time of writing is SDL_ttf, and it's compatible with the Fedora packages.\n"
		"\n"
		"Extract all files into your MingW directory.")
endif()
message("\n"
	"Scintilla will have to be built from sources.\n"
	"${CYAN}**************************************-INSTALLING-PACKAGES-*************************************${NONE}\n")
message(STATUS	"${BROWN}CMAKE_ROOT: ${CMAKE_ROOT}${NONE}")
message(STATUS	"${BROWN}CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}${NONE}")

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

option(AMANITA_LIBS "Build Amanita Libraries" ON)
option(AMANITA_BINS "Build Amanita Binaries" ON)
option(AMANITA_EXAMPLES "Build Amanita Library examples" ON)
option(ENABLE_NLS "Enable building of translations" ON)

check_include_file("dlfcn.h" HAVE_DLFCN_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("memory.h" HAVE_MEMORY_H)
check_include_file("stdbool.h" HAVE_STDBOOL_H)
check_include_file("stddef.h" HAVE_STDDEF_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("unistd.h" HAVE_UNISTD_H)

pkg_check_modules(SCINTILLA scintilla)
if(SCINTILLA_FOUND)
	set(HAVE_SCINTILLA 1)
	include_directories(${SCINTILLA_INCLUDE_DIRS})
	link_directories(${SCINTILLA_LIBRARY_DIRS})
	add_definitions(${SCINTILLA_CFLAGS_OTHER})
else()
	message("\n${RED}To build with Scintilla, get sources from: ${BLUE}http://www.scintilla.org/${NONE}\n")
endif()

check_include_file("sqlite3.h" HAVE_SQLITE3_H)
if(NOT HAVE_SQLITE3_H)
	if(MINGW)
		message("\n${RED}Install Sqlite3 REM-package (see notice above).${NONE}\n")
	endif()
	message(FATAL_ERROR "${RED}Sqlite required!${NONE}")
endif()

#check_include_file("tiffio.h" HAVE_TIFF_H)
#if(NOT HAVE_TIFF_H)
#	if(MINGW)
#		message(FATAL_ERROR "Find and install the Devpak named 'libtiff'.")
#	endif()
#endif()

check_include_file("libavcodec/avcodec.h" HAVE_LIBAVCODEC_H)
if(HAVE_LIBAVCODEC_H)
	set(HAVE_AVCODEC 1)
	set(AVCODEC_LIBS avcodec avformat swscale avutil)
else()
	if(LINUX)
		message("\n${RED}sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev${NONE}\n")
	elseif(MINGW)
		message("\n${RED}Install ffmpeg (see notice above).${NONE}\n")
	endif()
endif()

check_function_exists(realloc HAVE_REALLOC)
check_function_exists(malloc HAVE_MALLOC)
check_function_exists(floor HAVE_FLOOR)
check_function_exists(pow HAVE_POW)
check_function_exists(exp HAVE_EXP)
check_function_exists(log HAVE_LOG)
check_function_exists(sqrt HAVE_SQRT)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(open_memstream HAVE_OPEN_MEMSTREAM)
check_function_exists(strchr HAVE_STRCHR)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strnicmp HAVE_STRNICMP)
check_function_exists(strpbrk HAVE_STRPBRK)
check_function_exists(strstr HAVE_STRSTR)
check_function_exists(vprintf HAVE_VPRINTF)

find_package(GTK2 2.6)
if(GTK2_FOUND)
	include_directories(${GTK2_INCLUDE_DIRS})
	add_definitions(-DGTK)
else()
	if(LINUX)
		message("\n${RED}sudo apt-get install libgtk2.0-dev${NONE}\n")
		message(FATAL_ERROR "${RED}GTK+ 2.6 Required!${NONE}")
	elseif(MINGW)
		message("\n${RED}Install GTK+ REM-package (see notice above).${NONE}\n")
	endif()
endif()

pkg_check_modules(WEBKIT webkit-1.0)
if(WEBKIT_FOUND)
	include_directories(${WEBKIT_INCLUDE_DIRS})
	link_directories(${WEBKIT_LIBRARY_DIRS})
	add_definitions(${WEBKIT_CFLAGS_OTHER})
elseif(LINUX)
	message("\n${RED}sudo apt-get install libwebkit-dev${NONE}\n")
elseif(MINGW)
	message("\n${RED}Install LibWebKitGTK REM-package (see notice above).${NONE}\n")
endif()

pkg_check_modules(SOURCEVIEW gtksourceview-2.0)
if(SOURCEVIEW_FOUND)
	set(HAVE_SOURCEVIEW 1)
	include_directories(${SOURCEVIEW_INCLUDE_DIRS})
	link_directories(${SOURCEVIEW_LIBRARY_DIRS})
	add_definitions(${SOURCEVIEW_CFLAGS_OTHER})
endif()

find_package(SDL)
if(SDL_FOUND)
	include_directories(${SDL_INCLUDE_DIRS})
	message(STATUS "Build with: ${BROWN}SDL${NONE}")
	set(HAVE_SDL 1)
	find_package(SDL_image)
	if(SDLIMAGE_FOUND)
		message(STATUS "Build with: ${BROWN}SDL_image${NONE}")
		set(HAVE_SDL_IMG 1)
	elseif(LINUX)
		message("\n${RED}sudo apt-get install libsdl-image1.2-dev${NONE}\n")
	elseif(MINGW)
		message("\n"
			"${RED}Install SDL_image REM-package (see notice above).\n"
			"On error message: '-- Could NOT find SDLIMAGE (missing:  SDLIMAGE_INCLUDE_DIR)':\n"
			"Edit '[CMAKE_ROOT]/Modules/FindSDL_image.cmake', add 'include/SDL' to 'PATH_SUFFIXES'.${NONE}\n")
	endif()
	find_package(SDL_ttf)
	if(SDLTTF_FOUND)
		message(STATUS "Build with: ${BROWN}SDL_ttf${NONE}")
		set(HAVE_SDL_TTF 1)
	elseif(LINUX)
		message("\n${RED}sudo apt-get install libsdl-ttf2.0-dev${NONE}\n")
	elseif(MINGW)
		message("\n"
			"${RED}Download and install latest DevPak (see notice above).\n"
			"On error message: '-- Could NOT find SDLTTF (missing:  SDLTTF_INCLUDE_DIR)':\n"
			"Edit '[CMAKE_ROOT]/Modules/FindSDL_ttf.cmake', add 'include/SDL' to 'PATH_SUFFIXES'.${NONE}\n")
	endif()
elseif(LINUX)
	message("\n${RED}sudo apt-get install libsdl1.2-dev${NONE}\n")
elseif(MINGW)
	message("\n${RED}Install SDL1.2 REM-package (see notice above).${NONE}\n")
endif()

find_package(JPEG)
if(JPEG_FOUND)
	set(HAVE_JPEG_H 1)
#	if(MINGW)
#		set(JPEG_LIBRARIES ${JPEG_LIBRARIES} jbig z)
#	endif()
elseif(LINUX)
	message("\n${RED}sudo apt-get install libjpeg-dev${NONE}\n")
elseif(MINGW)
	message("\n${RED}Install Jpeg REM-package (see notice above).${NONE}\n")
endif()

find_package(TIFF)
if(TIFF_FOUND)
	set(HAVE_TIFF_H 1)
	if(MINGW)
		set(TIFF_LIBRARIES ${TIFF_LIBRARIES} ${JPEG_LIBRARIES})
	endif()
elseif(LINUX)
	message("\n${RED}sudo apt-get install libtiff5-dev${NONE}\n")
elseif(MINGW)
	message("\n${RED}Install Tiff REM-package (see notice above).${NONE}\n")
endif()

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_BINARY_DIR}/include"
)

if(CMAKE_TOOLCHAIN_FILE)
endif()

if(CMAKE_C_COMPILER_ID STREQUAL GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti")
	if(WIN32)
		set(CMAKE_CXX_FLAGS_RELEASE "-O2")
		set(CMAKE_CXX_FLAGS_DEBUG  "-Werror -g -gdwarf-2 -O0")
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "-O2")
		set(CMAKE_CXX_FLAGS_DEBUG  "-Werror -O0 -g3")
	endif()
endif()

add_definitions(-D__STDC_CONSTANT_MACROS)

if(UNIX)
	set(USE_UNIX 1)
	if(APPLE)
		message(STATUS "Platform: ${BROWN}Apple${NONE}")
		set(GUI "Cocoa")
		set(GUI_COCOA 1)
		set(USE_COCOA 1)
		set(USE_APPLE 1)
	else()
		message(STATUS "Platform: ${BROWN}Unix or Linux${NONE}")
		set(USE_LINUX 1)
		set(GUI "GTK+")
		set(GUI_GTK 1)
		set(USE_GTK 1)
		set(USE_GLIB 1)

		set(PACKAGE_DATADIR ${CMAKE_INSTALL_PREFIX}/share/amanita)
		set(PACKAGE_ICONSDIR ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor)
		set(PACKAGE_LOCALEDIR ${CMAKE_INSTALL_PREFIX}/share/locale)
	endif()
	link_libraries(${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS})
elseif(WIN32)
	message(STATUS "Platform: ${BROWN}Windows${NONE}")
	set(UNICODE 1)
	set(USE_WCHAR 1)
	set(USE_WIN32 1)
	set(HAVE_DD 1)
	set(USE_DD 1)
	if(GTK2_FOUND)
		set(GUI "GTK+")
		set(GUI_GTK 1)
		set(USE_GTK 1)
		set(USE_GLIB 1)
	else ()
		set(GUI "Win32")
		set(GUI_WIN32 1)
	endif()
	link_libraries(intl comctl32 ws2_32 ole32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)

	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
	enable_language(RC)

	if(MSVC)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -subsystem:windows")
	elseif(CMAKE_C_COMPILER_ID STREQUAL GNU)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
	endif()
	if(MINGW)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
		set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
		set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -static-libgcc -static-libstdc++ -s")
	endif()

	set(PACKAGE_DATADIR "data")
	set(PACKAGE_ICONSDIR "icons")
	set(PACKAGE_LOCALEDIR "data\\locale")
else()
	message(STATUS "Platform: ${BROWN}Unknown${NONE}")
	set(GUI "Unknown")
endif()


if(UNIX)
	if(APPLE)
	else()
		install(FILES icons/32x32/amanita.xpm DESTINATION share/pixmaps)
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



message(STATUS "GUI system: ${BROWN}${GUI}${NONE}")


if(AMANITA_LIBS)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	set(amanita_src
		src/amanita/Collection.cpp
		src/amanita/Colorcycle.cpp
		src/amanita/Config.cpp
		src/amanita/Dictionary.cpp
		src/amanita/File.cpp
		src/amanita/Flood.cpp
		src/amanita/Hashtable.cpp
		src/amanita/Integer.cpp
		src/amanita/Main.cpp
		src/amanita/Math.cpp
		src/amanita/Object.cpp
		src/amanita/Properties.cpp
		src/amanita/Random.cpp
		src/amanita/Regex.cpp
		src/amanita/String.cpp
		src/amanita/Thread.cpp
		src/amanita/Vector.cpp
		src/amanita/Wiki.cpp
		src/amanita/db/Database.cpp
		src/amanita/db/Record.cpp
		src/amanita/db/Result.cpp
		src/amanita/game/Map.cpp
		src/amanita/game/Path.cpp
		src/amanita/game/Planet.cpp
		src/amanita/game/Word.cpp
		src/amanita/gui/Application.cpp
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
		src/amanita/media/Video.cpp
		src/amanita/net/Client.cpp
		src/amanita/net/Http.cpp
		src/amanita/net/Server.cpp
		src/amanita/net/Socket.cpp
		src/amanita/set/Fractal.cpp
		src/amanita/tk/Application.cpp
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
		include/amanita/Bytes.h
		include/amanita/Collection.h
		include/amanita/Colorcycle.h
		${PROJECT_BINARY_DIR}/include/amanita/Config.h
		include/amanita/Dictionary.h
		include/amanita/File.h
		include/amanita/Flood.h
		include/amanita/Fontmetrics.h
		include/amanita/Geometry.h
		include/amanita/Hashtable.h
		include/amanita/Integer.h
		include/amanita/Main.h
		include/amanita/Math.h
		include/amanita/Object.h
		include/amanita/Properties.h
		include/amanita/Random.h
		include/amanita/Regex.h
		${PROJECT_BINARY_DIR}/include/amanita/Resource.h
		include/amanita/String.h
		include/amanita/Thread.h
		include/amanita/Vector.h
		include/amanita/Wiki.h
		DESTINATION include/amanita
	)

	install(FILES
		include/amanita/db/Database.h
		include/amanita/db/Record.h
		include/amanita/db/Result.h
		DESTINATION include/amanita/db
	)

	install(FILES
		include/amanita/game/Map.h
		include/amanita/game/Path.h
		include/amanita/game/Planet.h
		include/amanita/game/Word.h
		DESTINATION include/amanita/game
	)

	install(FILES
		include/amanita/gui/Application.h
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
		include/amanita/media/Video.h
		DESTINATION include/amanita/media
	)

	install(FILES
		include/amanita/net/Client.h
		include/amanita/net/Http.h
		include/amanita/net/Server.h
		include/amanita/net/Socket.h
		DESTINATION include/amanita/net
	)

	install(FILES
		include/amanita/set/Fractal.h
		DESTINATION include/amanita/set
	)

	install(FILES
		include/amanita/tk/Application.h
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
		include/ipomoea/bf.h
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
endif()

if(AMANITA_BINS)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

	add_executable(bf src/ipomoea/bf.c)
	set_target_properties(bf PROPERTIES COMPILE_DEFINITIONS "BF=1")

	add_executable(bfc src/ipomoea/bf.c)
	set_target_properties(bfc PROPERTIES COMPILE_DEFINITIONS "BFC=1")

	install(TARGETS
		bf
		bfc
		DESTINATION bin
	)
endif()

if(AMANITA_EXAMPLES)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/examples)

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
	if(USE_GTK)
		target_link_libraries(browser amanita ${GTK2_LIBRARIES} ${WEBKIT_LIBRARIES})
	elseif(WIN32)
		target_link_libraries(browser amanita)
	endif()

	add_executable(client examples/client.cpp)
	target_link_libraries(client amanita ${CMAKE_THREAD_LIBS_INIT})

	add_executable(database examples/database.cpp)
	target_link_libraries(database amanita sqlite3)

	add_executable(entropy examples/entropy.cpp)
	target_link_libraries(entropy amanita)

	if(HAVE_DD)
		add_executable(fractal WIN32 examples/fractal.cpp ${RC_FILE})
		target_link_libraries(fractal amanita ${AVCODEC_LIBS} ${TIFF_LIBRARIES} ddraw dxguid uuid olepro32 ole32)
	elseif(HAVE_SDL)
		add_executable(fractal examples/fractal.cpp)
		target_link_libraries(fractal amanita ${AVCODEC_LIBS} ${TIFF_LIBRARIES} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY})
	endif()

	if(WIN32)
		add_executable(gui WIN32 examples/gui.cpp ${RC_FILE})
	else()
		add_executable(gui examples/gui.cpp)
	endif()
	if(USE_GTK)
		target_link_libraries(gui amanita ${SCINTILLA_LIBRARIES} ${SOURCEVIEW_LIBRARIES} ${WEBKIT_LIBRARIES} ${GTK2_LIBRARIES})
	else()
		target_link_libraries(gui amanita ${SCINTILLA_LIBRARIES})
	endif()

	add_executable(hashtable examples/hashtable.cpp)
	target_link_libraries(hashtable amanita)

	add_executable(horoscope examples/horoscope.c)
	target_link_libraries(horoscope mandragora m)

	add_executable(http examples/http.cpp)
	target_link_libraries(http amanita ${CMAKE_THREAD_LIBS_INIT})

	if(HAVE_DD)
		add_executable(mandala WIN32 examples/mandala.cpp ${RC_FILE})
		target_link_libraries(mandala amanita ddraw dxguid uuid olepro32 ole32)
	elseif(HAVE_SDL)
		add_executable(mandala examples/mandala.cpp)
		target_link_libraries(mandala amanita ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY})
	endif()

	if(HAVE_DD)
		add_executable(path WIN32 examples/path.cpp ${RC_FILE})
		target_link_libraries(path amanita ddraw dxguid uuid olepro32 ole32)
	elseif(HAVE_SDL)
		add_executable(path examples/path.cpp)
		target_link_libraries(path amanita ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY})
	endif()

	if(HAVE_DD)
		add_executable(planet WIN32 examples/planet.cpp ${RC_FILE})
		target_link_libraries(planet amanita ${AVCODEC_LIBS} ddraw dxguid uuid olepro32 ole32)
	elseif(HAVE_SDL)
		add_executable(planet examples/planet.cpp)
		target_link_libraries(planet amanita ${AVCODEC_LIBS} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY})
	endif()

	add_executable(properties examples/properties.cpp)
	target_link_libraries(properties amanita)
	add_custom_command(TARGET properties
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/examples/properties.cfg ${PROJECT_BINARY_DIR}/examples/properties.cfg
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/examples/properties.ini ${PROJECT_BINARY_DIR}/examples/properties.ini
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/examples/properties.properties ${PROJECT_BINARY_DIR}/examples/properties.properties
	)

	add_executable(regex examples/regex.cpp)
	target_link_libraries(regex amanita)

	add_executable(server examples/server.cpp)
	target_link_libraries(server amanita ${CMAKE_THREAD_LIBS_INIT})

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

