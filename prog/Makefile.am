AM_CFLAGS = $(DEBUG_FLAGS)
AM_CPPFLAGS = \
	-I../src \
	$(LEPT_CFLAGS) \
	$(LUA_CFLAGS)

INSTALL_PROGS = llua
bin_PROGRAMS = $(INSTALL_PROGS)
llua_SOURCES = llua.cpp ../src/lualept.h
llua_LDADD = $(top_builddir)/src/liblualept.la
