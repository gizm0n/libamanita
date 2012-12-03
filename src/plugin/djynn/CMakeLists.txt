
# CMakeLists.txt


cmake_minimum_required(VERSION 2.8)
project(djynn) 

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

set(PACKAGE "djynn")
set(PACKAGE_NAME "Djynn Geany Plugin")
set(PACKAGE_VERSION_MAJOR 1)
set(PACKAGE_VERSION_MINOR 1)
set(PACKAGE_VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.0")
set(PACKAGE_BUGREPORT "per.lowgren@gmail.com")
set(PACKAGE_URL "http://code.google.com/p/libamanita/")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE}")


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

include_directories(
	"${PROJECT_SOURCE_DIR}/../../../include"
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
	else()
		message("Unknown")
		set(GUI "Unknown")
	endif()
endif()


message("GUI system is ${GUI}")



PKG_CHECK_MODULES(GEANY geany)
if(GEANY_FOUND)
	set(ipomoea_src
		../../ipomoea/base64.c
		../../ipomoea/hashtable_t.c
		../../ipomoea/rle.c
		../../ipomoea/string_t.c
		../../ipomoea/vector_t.c
	)

	message("Has Geany installed")
	include_directories(${GEANY_INCLUDE_DIRS})
	link_directories(${GEANY_LIBRARY_DIRS})
	add_definitions(${GEANY_CFLAGS_OTHER})

	set(djynn_src
		./projectmanager_dlg.c
		./projectmanager.c
		./base64.c
		./comment.c
		./sort.c
		./djynn.c
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
	message("Does not have Geany installed")
endif()



