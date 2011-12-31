

CXXFLAGS = -Wall -Werror -O2 -fno-strict-aliasing -Wno-unused-but-set-variable -g -I../include/

lib_LIBRARIES = libamanita.a libamanita-sdl.a


nobase_include_HEADERS = \
	../include/amanita/aApplication.h \
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
	../include/amanita/gui/aEvent.h \
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
	../src/aApplication.cpp \
	../src/aCollection.cpp \
	../src/aDictionary.cpp \
	../src/aFile.cpp \
	../src/aFlood.cpp \
	../src/aHashtable.cpp \
	../src/aInteger.cpp \
	../src/aMath.cpp \
	../src/aObject.cpp \
	../src/aPath.cpp \
	../src/aRandom.cpp \
	../src/aRegex.cpp \
	../src/aString.cpp \
	../src/aString_HTMLentities.cpp \
	../src/aThread.cpp \
	../src/aVector.cpp \
	../src/aWord.cpp \
	../src/config.cpp \
	../src/config.h \
	../src/net/aClient.cpp \
	../src/net/aHttp.cpp \
	../src/net/aServer.cpp \
	../src/net/aSocket.cpp \
	../src/sqlite/aRecord.cpp \
	../src/sqlite/aResultset.cpp \
	../src/sqlite/aSQLite.cpp

libamanita_a_CPPFLAGS = \
	-DLIBAMANITA \
	-DLIBAMANITA_DEBUG
libamanita_a_CXXFLAGS = `pkg-config --cflags gtk+-2.0`


libamanita_sdl_a_SOURCES = \
	../src/aApplication.cpp \
	../src/aCollection.cpp \
	../src/aDictionary.cpp \
	../src/aFile.cpp \
	../src/aFlood.cpp \
	../src/aHashtable.cpp \
	../src/aInteger.cpp \
	../src/aMath.cpp \
	../src/aObject.cpp \
	../src/aPath.cpp \
	../src/aRandom.cpp \
	../src/aRegex.cpp \
	../src/aString.cpp \
	../src/aString_HTMLentities.cpp \
	../src/aThread.cpp \
	../src/aVector.cpp \
	../src/aWord.cpp \
	../src/config.cpp \
	../src/config.h \
	../src/net/aClient.cpp \
	../src/net/aHttp.cpp \
	../src/net/aServer.cpp \
	../src/net/aSocket.cpp \
	../src/sdl/aButton.cpp \
	../src/sdl/aComponent.cpp \
	../src/sdl/aDisplay.cpp \
	../src/sdl/aFont.cpp \
	../src/sdl/aGraphics.cpp \
	../src/sdl/aIconButton.cpp \
	../src/sdl/aImage.cpp \
	../src/sdl/aScroll.cpp \
	../src/sdl/aTabset.cpp \
	../src/sdl/aText.cpp \
	../src/sdl/aTextbox.cpp \
	../src/sdl/aTextfield.cpp \
	../src/sdl/aTooltip.cpp \
	../src/sdl/aWindow.cpp \
	../src/sqlite/aRecord.cpp \
	../src/sqlite/aResultset.cpp \
	../src/sqlite/aSQLite.cpp

libamanita_sdl_a_CPPFLAGS = \
	-DLIBAMANITA \
	-DLIBAMANITA_SDL \
	-DLIBAMANITA_DEBUG
libamanita_sdl_a_CXXFLAGS = `pkg-config --cflags gtk+-2.0`


