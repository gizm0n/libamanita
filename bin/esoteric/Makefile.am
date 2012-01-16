

CFLAGS =  @AM_CFLAGS@ -I../include/
LDFLAGS = @AM_LDFLAGS@


lib_LIBRARIES = libesoteric.a


nobase_include_HEADERS = \
	../include/esoteric/astro/astrology.h \
	../include/esoteric/astro/astronomy.h \
	../include/esoteric/astro/horoscope.h \
	../include/esoteric/calendar.h

libesoteric_a_SOURCES = \
	../src/calendar.c \
	../src/config.c \
	../src/config.h \
	../src/astro/planets/earth404.c \
	../src/astro/planets/jupiter404.c \
	../src/astro/planets/mars404.c \
	../src/astro/planets/mercury404.c \
	../src/astro/planets/moon404.c \
	../src/astro/planets/neptune404.c \
	../src/astro/planets/pluto404.c \
	../src/astro/planets/saturn404.c \
	../src/astro/planets/uranus404.c \
	../src/astro/planets/venus404.c \
	../src/astro/astrology.c \
	../src/astro/astronomy.c \
	../src/astro/astronomy_data.c \
	../src/astro/astronomy_data.h \
	../src/astro/horoscope.c

libesoteric_a_CFLAGS = \
	-DLIBESOTERIC \
	-DLIBESOTERIC_DEBUG


