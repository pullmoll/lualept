AC_PREREQ([2.50])
AC_INIT([lualept], [0.5.0])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([src/config.h:config/config.h.in])
AC_CONFIG_SRCDIR([src/lualept.cpp])

AC_CONFIG_MACRO_DIR([m4])
LT_INIT

AM_INIT_AUTOMAKE
AC_LANG(C++)
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# Checks for libraries.
LT_LIB_M

# Checks for pkg-config libraries.
PKG_CHECK_MODULES([LEPT], [lept >= 1.76.0])
PKG_CHECK_MODULES([LUA],  [lua5.3 >= 5.3.0])

# Optionally link -lSDL2 to support internal Pix display
AC_ARG_WITH([sdl2], AS_HELP_STRING([--with-sdl2=no], [Disable building the internal SDL2 viewer.]))
if test "x$with_sdl2" != "xno"; then
	PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.0],
		[AC_DEFINE([HAVE_SDL2], [1], [Define to 1 if you have the <SDL2/SDL.h> header file.])])
fi

case "$host_os" in
  mingw32*)
  AC_SUBST([GDI_LIBS], [-lgdi32])
  CPPFLAGS="${CPPFLAGS} -D__USE_MINGW_ANSI_STDIO" ;;
esac

# Enable less verbose output when building.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADERS([ctype.h errno.h float.h limits.h time.h sys/time.h])
AC_TYPE_SIZE_T
AC_C_BIGENDIAN

# Checks for library functions.
AC_CHECK_FUNCS([strcasecmp stricmp gettimeofday localtime_r localtime gmtime_r gmtime])

AC_CONFIG_FILES([Makefile src/Makefile prog/Makefile lualept.pc Doxyfile])
AC_OUTPUT
