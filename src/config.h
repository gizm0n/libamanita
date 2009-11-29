#ifndef _CONFIG_H
#define _CONFIG_H


#ifdef __linux__
#	include "../config.h"
#endif


#ifndef HAVE_STRNICMP
#	define strnicmp strncasecmp
#endif


#define __STDC_FORMAT_MACROS
#ifdef __linux__
#	if _WORDSIZE == 64
#		define PRIl			"l"
#		define PRIll		"l"
#	else
#		define PRIl			"l"
#		define PRIll		"ll"
#	endif
#		define PRIlf		"L"
#elif defined _WIN32
#	ifdef _WIN64
#		define PRIl			"l"
#		define PRIll		"l"
#	else
#		define PRIl			"l"
#		define PRIll		"I64l"
#	endif
#		define PRIlf		"L"
#endif


#endif /* _CONFIG_H */


