
# this one is important
set(CMAKE_SYSTEM_NAME Windows)
#this one not so much
set(CMAKE_SYSTEM_VERSION 1)


set(CMAKE_INSTALL_PREFIX /usr/i686-w64-mingw32 CACHE STRING "Set Install Prefix." FORCE)
set(CPACK_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

# specify the cross compiler
set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
set(CMAKE_RC_COMPILER i686-w64-mingw32-windres)

# where is the target environment 
set(CMAKE_FIND_ROOT_PATH /usr/i686-w64-mingw32)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(ENV{PKG_CONFIG_LIBDIR} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig/)

