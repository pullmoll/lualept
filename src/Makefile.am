AM_CFLAGS = $(DEBUG_FLAGS)
AM_CPPFLAGS = -DLUALEPT_LIB $(LEPT_CFLAGS) $(LUA_CFLAGS) $(SDL2_CFLAGS)

lib_LTLIBRARIES = liblualept.la

liblualept_la_LDFLAGS = -no-undefined -version-info 1:0:0
liblualept_la_CFLAGS = $(LEPT_CFLAGS) $(LUA_CFLAGS) $(SDL2_CFLAGS)
liblualept_la_LIBADD = $(LEPT_LIBS) $(LUA_LIBS) $(SDL2_LIBS)
liblualept_la_SOURCES = lualept.cpp lualept.h modules.h \
 llaset.cpp llamap.cpp llbmf.cpp lldllist.cpp \
 lldewarp.cpp lldewarpa.cpp \
 llnuma.cpp llnumaa.cpp \
 lldna.cpp lldnaa.cpp \
 llpta.cpp llptaa.cpp \
 llbox.cpp llboxa.cpp llboxaa.cpp \
 llpixcmap.cpp llpix.cpp llpixa.cpp llpixaa.cpp \
 llpixtiling.cpp llpixcomp.cpp llpixacomp.cpp \
 llfpix.cpp llfpixa.cpp lldpix.cpp \
 llsel.cpp llsela.cpp \
 llccbord.cpp llccborda.cpp \
 llkernel.cpp llstack.cpp \
 llcompdata.cpp llpdfdata.cpp

pkginclude_HEADERS = lualept.h

bin_PROGRAMS = llua
llua_SOURCES = llua.cpp
llua_LDADD = liblualept.la
