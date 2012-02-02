

CFLAGS =  $(AM_CFLAGS) -I../include/
CXXFLAGS =  $(AM_CXXFLAGS) -I../include/
LDFLAGS = $(AM_LDFLAGS)


lib_LIBRARIES = \
	libamanita.a \
	libamanita-sdl.a \
	libesoteric.a \
	libipomoea.a


nobase_include_HEADERS = \
	../include/amanita/aApplication.h \
	../include/amanita/aBytes.h \
	../include/amanita/aCollection.h \
	../include/amanita/aDictionary.h \
	../include/amanita/aFile.h \
	../include/amanita/aFlood.h \
	../include/amanita/aFontmetrics.h \
	../include/amanita/aHashtable.h \
	../include/amanita/aInteger.h \
	../include/amanita/aMath.h \
	../include/amanita/aObject.h \
	../include/amanita/aPath.h \
	../include/amanita/aRandom.h \
	../include/amanita/aRegex.h \
	../include/amanita/aString.h \
	../include/amanita/aThread.h \
	../include/amanita/aVector.h \
	../include/amanita/aWord.h \
	../include/amanita/gui/aBrowser.h \
	../include/amanita/gui/aEvent.h \
	../include/amanita/gui/aSizer.h \
	../include/amanita/gui/aWidget.h \
	../include/amanita/net/aClient.h \
	../include/amanita/net/aHttp.h \
	../include/amanita/net/aServer.h \
	../include/amanita/net/aSocket.h \
	../include/amanita/sdl/aButton.h \
	../include/amanita/sdl/aComponent.h \
	../include/amanita/sdl/aDisplay.h \
	../include/amanita/sdl/aFont.h \
	../include/amanita/sdl/aGraphics.h \
	../include/amanita/sdl/aIconButton.h \
	../include/amanita/sdl/aImage.h \
	../include/amanita/sdl/aScroll.h \
	../include/amanita/sdl/aTabset.h \
	../include/amanita/sdl/aText.h \
	../include/amanita/sdl/aTextbox.h \
	../include/amanita/sdl/aTextfield.h \
	../include/amanita/sdl/aTooltip.h \
	../include/amanita/sdl/aWindow.h \
	../include/amanita/sqlite/aRecord.h \
	../include/amanita/sqlite/aResultset.h \
	../include/amanita/sqlite/aSQLite.h

libamanita_a_SOURCES = \
	../src/amanita/aApplication.cpp \
	../src/amanita/aCollection.cpp \
	../src/amanita/aDictionary.cpp \
	../src/amanita/aFile.cpp \
	../src/amanita/aFlood.cpp \
	../src/amanita/aHashtable.cpp \
	../src/amanita/aInteger.cpp \
	../src/amanita/aMath.cpp \
	../src/amanita/aObject.cpp \
	../src/amanita/aPath.cpp \
	../src/amanita/aRandom.cpp \
	../src/amanita/aRegex.cpp \
	../src/amanita/aString.cpp \
	../src/amanita/aString_HTMLentities.cpp \
	../src/amanita/aThread.cpp \
	../src/amanita/aVector.cpp \
	../src/amanita/aWord.cpp \
	../src/amanita/config.cpp \
	../src/amanita/config.h \
	../src/amanita/gui/aBrowser.cpp \
	../src/amanita/gui/aSizer.cpp \
	../src/amanita/gui/aWidget.cpp \
	../src/amanita/net/aClient.cpp \
	../src/amanita/net/aHttp.cpp \
	../src/amanita/net/aServer.cpp \
	../src/amanita/net/aSocket.cpp \
	../src/amanita/sqlite/aRecord.cpp \
	../src/amanita/sqlite/aResultset.cpp \
	../src/amanita/sqlite/aSQLite.cpp

libamanita_a_CPPFLAGS = \
	-DLIBAMANITA \
	-DLIBAMANITA_DEBUG
libamanita_a_CXXFLAGS = $(DEPS_CFLAGS)


libamanita_sdl_a_SOURCES = \
	../src/amanita/aApplication.cpp \
	../src/amanita/aCollection.cpp \
	../src/amanita/aDictionary.cpp \
	../src/amanita/aFile.cpp \
	../src/amanita/aFlood.cpp \
	../src/amanita/aHashtable.cpp \
	../src/amanita/aInteger.cpp \
	../src/amanita/aMath.cpp \
	../src/amanita/aObject.cpp \
	../src/amanita/aPath.cpp \
	../src/amanita/aRandom.cpp \
	../src/amanita/aRegex.cpp \
	../src/amanita/aString.cpp \
	../src/amanita/aString_HTMLentities.cpp \
	../src/amanita/aThread.cpp \
	../src/amanita/aVector.cpp \
	../src/amanita/aWord.cpp \
	../src/amanita/config.cpp \
	../src/amanita/config.h \
	../src/amanita/gui/aBrowser.cpp \
	../src/amanita/gui/aSizer.cpp \
	../src/amanita/gui/aWidget.cpp \
	../src/amanita/net/aClient.cpp \
	../src/amanita/net/aHttp.cpp \
	../src/amanita/net/aServer.cpp \
	../src/amanita/net/aSocket.cpp \
	../src/amanita/sdl/aButton.cpp \
	../src/amanita/sdl/aComponent.cpp \
	../src/amanita/sdl/aDisplay.cpp \
	../src/amanita/sdl/aFont.cpp \
	../src/amanita/sdl/aGraphics.cpp \
	../src/amanita/sdl/aIconButton.cpp \
	../src/amanita/sdl/aImage.cpp \
	../src/amanita/sdl/aScroll.cpp \
	../src/amanita/sdl/aTabset.cpp \
	../src/amanita/sdl/aText.cpp \
	../src/amanita/sdl/aTextbox.cpp \
	../src/amanita/sdl/aTextfield.cpp \
	../src/amanita/sdl/aTooltip.cpp \
	../src/amanita/sdl/aWindow.cpp \
	../src/amanita/sqlite/aRecord.cpp \
	../src/amanita/sqlite/aResultset.cpp \
	../src/amanita/sqlite/aSQLite.cpp

libamanita_sdl_a_CPPFLAGS = \
	-DLIBAMANITA \
	-DLIBAMANITA_SDL \
	-DLIBAMANITA_DEBUG
libamanita_sdl_a_CXXFLAGS = $(DEPS_CFLAGS)


nobase_include_HEADERS += \
	../include/esoteric/astro/astrology.h \
	../include/esoteric/astro/astronomy.h \
	../include/esoteric/astro/horoscope.h \
	../include/esoteric/calendar.h

libesoteric_a_SOURCES = \
	../src/esoteric/calendar.c \
	../src/esoteric/config.c \
	../src/esoteric/config.h \
	../src/esoteric/astro/planets/earth404.c \
	../src/esoteric/astro/planets/jupiter404.c \
	../src/esoteric/astro/planets/mars404.c \
	../src/esoteric/astro/planets/mercury404.c \
	../src/esoteric/astro/planets/moon404.c \
	../src/esoteric/astro/planets/neptune404.c \
	../src/esoteric/astro/planets/pluto404.c \
	../src/esoteric/astro/planets/saturn404.c \
	../src/esoteric/astro/planets/uranus404.c \
	../src/esoteric/astro/planets/venus404.c \
	../src/esoteric/astro/astrology.c \
	../src/esoteric/astro/astronomy.c \
	../src/esoteric/astro/astronomy_data.c \
	../src/esoteric/astro/astronomy_data.h \
	../src/esoteric/astro/horoscope.c

libesoteric_a_CFLAGS = \
	-DLIBESOTERIC \
	-DLIBESOTERIC_DEBUG


nobase_include_HEADERS += \
	../include/ipomoea/hashtable_t.h \
	../include/ipomoea/string_t.h \
	../include/ipomoea/vector_t.h

libipomoea_a_SOURCES = \
	../src/ipomoea/hashtable_t.c \
	../src/ipomoea/string_t.c \
	../src/ipomoea/vector_t.c




