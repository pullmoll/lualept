AM_CFLAGS = $(DEBUG_FLAGS)
AM_CPPFLAGS = -DLUALEPT_LIB $(LEPT_CFLAGS) $(LUA_CFLAGS)

lib_LTLIBRARIES = liblualept.la

liblualept_la_LDFLAGS = -no-undefined -version-info 1:0:0
liblualept_la_CFLAGS = $(LEPT_CFLAGS) $(LUA_CFLAGS)
liblualept_la_LIBADD = $(LEPT_LIBS) $(LUA_LIBS)
liblualept_la_SOURCES = lualept.cpp lualept.h modules.h \
 llaset.cpp llamap.cpp \
 lllist.cpp \
 llnuma.cpp llnumaa.cpp \
 lldna.cpp lldnaa.cpp \
 llpta.cpp llptaa.cpp \
 llbox.cpp llboxa.cpp llboxaa.cpp \
 llpixcmap.cpp llpix.cpp llpixa.cpp llpixaa.cpp

pkginclude_HEADERS = lualept.h

bin_PROGRAMS = llua
llua_SOURCES = llua.cpp
llua_LDADD = liblualept.la
