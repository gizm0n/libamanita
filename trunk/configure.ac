#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.68)
AC_INIT([Amanita Library],[0.01],[per.lowgren@gmail.com],[libamanita])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/config.h])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
case "$host" in
	*linux*)
		PKG_CHECK_MODULES([DEPS],[gtk+-2.0 webkit-1.0])
		PKG_CHECK_MODULES([DJYNN],[geany])
		AM_CFLAGS="-g -Wall -Werror -O0 -fno-strict-aliasing -Wno-uninitialized -Wno-unused-but-set-variable"
		AM_CXXFLAGS="$AM_CFLAGS"
		AM_LDFLAGS=""
		EXAMPLES_LDFLAGS="-pthread"
		EXAMPLES_LIBS=" -lm"
		;;
	*mingw32*)
		AM_CFLAGS="-g -Wall -Werror -O0 -fno-strict-aliasing -Wno-uninitialized -mms-bitfields"
		AM_CXXFLAGS="$AM_CFLAGS"
		AM_LDFLAGS="-Wl,-subsystem,windows"
		EXAMPLES_LDFLAGS=""
		EXAMPLES_LIBS="-lws2_32 -lcomctl32 -lwinmm -lole32 -loleaut32 -luuid"
		;;
	*)
		AM_CFLAGS="$AM_CFLAGS"
		AM_CXXFLAGS="$os_cflags"
		AM_LDFLAGS=""
		EXAMPLES_LDFLAGS=""
		EXAMPLES_LIBS=""
		;;
esac
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([EXAMPLES_LDFLAGS])
AC_SUBST([EXAMPLES_LIBS])


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h stdint.h inttypes.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_INT16_T
AC_TYPE_UINT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([floor memset pow sqrt strchr strdup strpbrk strstr strnicmp])

AC_OUTPUT([
	Makefile
	bin/Makefile
	examples/Makefile
])


