

CFLAGS =  $(AM_CFLAGS) $(DEPS_CFLAGS)
CXXFLAGS =  $(AM_CXXFLAGS) $(DEPS_CFLAGS)
LDFLAGS = $(AM_LDFLAGS) $(EXAMPLES_LDFLAGS)

noinst_PROGRAMS = \
	browser \
	client \
	horoscope \
	http \
	server

browser_SOURCES = ./browser.cpp
browser_LDADD = -lamanita $(EXAMPLES_LIBS) $(DEPS_LIBS)

client_SOURCES = ./client.cpp
client_LDADD = -lamanita $(EXAMPLES_LIBS) $(DEPS_LIBS)

horoscope_SOURCES = ./horoscope.c
horoscope_LDADD = -lesoteric $(EXAMPLES_LIBS) $(DEPS_LIBS)

http_SOURCES = ./http.cpp
http_LDADD = -lamanita $(EXAMPLES_LIBS) $(DEPS_LIBS)

server_SOURCES = ./server.cpp
server_LDADD = -lamanita $(EXAMPLES_LIBS) $(DEPS_LIBS)





