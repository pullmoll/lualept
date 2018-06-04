AM_CFLAGS = $(DEBUG_FLAGS)
AM_CPPFLAGS = -DLUALEPT_LIB $(LEPT_CFLAGS) $(LUA_CFLAGS) $(SDL2_CFLAGS)

lib_LTLIBRARIES = liblualept.la

liblualept_la_LDFLAGS = -no-undefined -version-info 1:0:0
liblualept_la_CFLAGS = $(LEPT_CFLAGS) $(LUA_CFLAGS) $(SDL2_CFLAGS)
liblualept_la_LIBADD = $(LEPT_LIBS) $(LUA_LIBS) $(SDL2_LIBS)
liblualept_la_SOURCES = \
	lualept.cpp \
	lualept-sdl2.cpp \
	lualept.h \
	modules.h \
	llamap.cpp \
	llaset.cpp \
	llbbuffer.cpp \
	llbmf.cpp \
	llbox.cpp \
	llboxa.cpp \
	llboxaa.cpp \
	llccbord.cpp \
	llccborda.cpp \
	llcompdata.cpp \
	lldewarp.cpp \
	lldewarpa.cpp \
	lldllist.cpp \
	lldna.cpp \
	lldnaa.cpp \
	lldpix.cpp \
	llfpix.cpp \
	llfpixa.cpp \
	llkernel.cpp \
	llnuma.cpp \
	llnumaa.cpp \
	llpdfdata.cpp \
	llpix.cpp \
	llpixa.cpp \
	llpixaa.cpp \
	llpixacomp.cpp \
	llpixcmap.cpp \
	llpixcomp.cpp \
	llpixtiling.cpp \
	llpta.cpp \
	llptaa.cpp \
	llsel.cpp \
	llsela.cpp \
	llstack.cpp \
	llwshed.cpp

pkginclude_HEADERS = lualept.h

bin_PROGRAMS = llua
llua_SOURCES = llua.cpp
llua_LDADD = liblualept.la
