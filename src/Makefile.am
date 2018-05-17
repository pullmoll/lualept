AM_CFLAGS = $(DEBUG_FLAGS)
AM_CPPFLAGS = -DLUALEPT_LIB $(LEPT_CFLAGS) $(LUA_CFLAGS)

lib_LTLIBRARIES = liblualept.la

liblualept_la_LDFLAGS = -no-undefined -version-info 1:0:0
liblualept_la_CFLAGS = $(LEPT_CFLAGS) $(LUA_CFLAGS)
liblualept_la_LIBADD = $(LEPT_LIBS) $(LUA_LIBS)
liblualept_la_SOURCES = lualept.c llnuma.c llnumaa.c lldna.c lldnaa.c   \
 llpta.c llptaa.c llaset.c llamap.c lllist.c llbox.c llboxa.c llboxaa.c \
 llpixcmap.c llpix.c llpixa.c llpixaa.c lualept.h modules.h
pkginclude_HEADERS = lualept.h

bin_PROGRAMS = llua
llua_SOURCES = llua.c
llua_LDADD = liblualept.la
