
# CMakeLists.txt


cmake_minimum_required(VERSION 2.8)
project(djynn) 

set(PACKAGE "djynn")
set(PACKAGE_NAME "Djynn Geany Plugin")
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

message(STATUS "${BROWN}CMAKE_ROOT: ${CMAKE_ROOT}${NONE}")
message(STATUS "${BROWN}CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}${NONE}")

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

find_package(GTK2 2.6)
if(GTK2_FOUND)
	include_directories(${GTK2_INCLUDE_DIRS})
	add_definitions(-DGTK)
else()
	if(MINGW)
		message("\n${RED}Download GTK+ at: ${BLUE}http://www.gtk.org/download/win32.php${NONE}\n")
	endif()
	message(FATAL_ERROR "${RED}GTK+ 2.6 Required!${NONE}")
endif()

PKG_CHECK_MODULES(GIO "gio-2.0 >= 2.26.0")
if(GIO_FOUND)
	include_directories(${GIO_INCLUDE_DIRS})
	link_directories(${GIO_LIBRARY_DIRS})
	add_definitions(-DHAVE_GIO)
endif()

include_directories(
	"${PROJECT_SOURCE_DIR}/../../../include"
	"${PROJECT_BINARY_DIR}/include"
)

if(CMAKE_TOOLCHAIN_FILE)
endif()

if(CMAKE_C_COMPILER_ID STREQUAL GNU)
	set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	if(WIN32)
		set(CMAKE_C_FLAGS_RELEASE "-O2")
		set(CMAKE_C_FLAGS_DEBUG  "-Werror -O0 -gdwarf-2 -g3")
	else()
		set(CMAKE_C_FLAGS_RELEASE "-O2")
		set(CMAKE_C_FLAGS_DEBUG  "-Werror -O0 -g3")
	endif()
endif()

if(UNIX)
	set(USE_UNIX 1)
	if(APPLE)
		message(STATUS "${BROWN}Apple${NONE}")
		set(GUI "Cocoa")
		set(GUI_COCOA 1)
		set(USE_COCOA 1)
		set(USE_APPLE 1)
	else()
		message(STATUS "${BROWN}Unix or Linux${NONE}")
		set(GUI "GTK+")
		set(GUI_GTK 1)
		set(USE_GTK 1)
		set(USE_GLIB 1)
	endif()
	link_libraries(${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS})
else()
	if(WIN32)
		message(STATUS "${BROWN}Windows${NONE}")
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
			set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
		endif()
	else()
		message(STATUS "${BROWN}Unknown OS${NONE}")
		set(GUI "Unknown")
	endif()
endif()


message(STATUS "${BROWN}GUI system is ${GUI}${NONE}")



PKG_CHECK_MODULES(GEANY geany)
if(GEANY_FOUND)
	set(ipomoea_src
		../../ipomoea/base64.c
		../../ipomoea/bf.c
		../../ipomoea/hashtable_t.c
		../../ipomoea/rle.c
		../../ipomoea/string_t.c
		../../ipomoea/vector_t.c
	)

	message(STATUS "${BROWN}Has Geany installed${NONE}")
	include_directories(${GEANY_INCLUDE_DIRS})
	link_directories(${GEANY_LIBRARY_DIRS})
	add_definitions(${GEANY_CFLAGS_OTHER})

	set(djynn_src
		./projectmanager_dlg.c
		./projectmanager.c
		./ascii.c
		./base64.c
		./bf.c
		./comment.c
		./sort.c
		./djynn.c
	)
	add_library(djynn SHARED ${djynn_src})
	set_target_properties(djynn PROPERTIES PREFIX "")

	if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
		message(STATUS "${BROWN}Build Djynn with -fPIC flag${NONE}")
		add_library(ipomoea_djynn_pic STATIC ${ipomoea_src})
		set_target_properties(ipomoea_djynn_pic PROPERTIES COMPILE_FLAGS -fpic)
		set_target_properties(djynn PROPERTIES COMPILE_FLAGS -fpic)
		target_link_libraries(djynn ipomoea_djynn_pic ${GTK_LIBRARIES} ${GEANY_LIBRARIES})
	else()
		message(STATUS "${BROWN}Build Djynn${NONE}")
		target_link_libraries(djynn ipomoea ${GTK2_LIBRARIES} ${GEANY_LIBRARIES})
	endif()

	install(TARGETS djynn DESTINATION "${GEANY_LIBDIR}/geany")

	# CPack installer
	include(InstallRequiredSystemLibraries)
	#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
	set(CPACK_PACKAGE_VERSION_MAJOR "${PACKAGE_VERSION_MAJOR}")
	set(CPACK_PACKAGE_VERSION_MINOR "${PACKAGE_VERSION_MINOR}")
	set(CPACK_GENERATOR "DEB")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Per Löwgren")
	include(CPack)
else()
	message(FATAL_ERROR "${RED}Does not have Geany installed${NONE}")
endif()



