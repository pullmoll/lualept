AC_PREREQ([2.50])
AC_INIT([lualept], [0.1.0])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([src/config.h:config/config.h.in])
AC_CONFIG_SRCDIR([src/lualept.cpp])

AC_CONFIG_MACRO_DIR([m4])
LT_INIT

AM_INIT_AUTOMAKE
AC_LANG(C)
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
LT_LIB_M

PKG_CHECK_MODULES([LEPT], [lept >= 1.76.0])
PKG_CHECK_MODULES([LUA], [lua5.3 >= 5.3.0])

case "$host_os" in
  mingw32*)
  AC_SUBST([GDI_LIBS], [-lgdi32])
  CPPFLAGS="${CPPFLAGS} -D__USE_MINGW_ANSI_STDIO" ;;
esac

# Enable less verbose output when building.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_HEADERS([ctype.h float.h limits.h])
AC_C_BIGENDIAN

# Checks for library functions.
AC_CHECK_FUNCS([strcasecmp stricmp])

AC_CONFIG_FILES([Makefile src/Makefile lualept.pc])
AC_OUTPUT
