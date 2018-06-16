################################################################################
#
# configure
#
################################################################################

########################################
# FUNCTION check_includes
########################################
function(check_includes files)
    foreach(F ${${files}})
	set(name ${F})
	string(REPLACE "-" "_" name ${name})
	string(REPLACE "." "_" name ${name})
	string(REPLACE "/" "_" name ${name})
	string(TOUPPER ${name} name)
	check_include_files(${F} HAVE_${name})
	file(APPEND ${AUTOCONFIG_SRC} "/* Define to 1 if you have the <${F}> header file. */\n")
	file(APPEND ${AUTOCONFIG_SRC} "#cmakedefine HAVE_${name} 1\n")
	file(APPEND ${AUTOCONFIG_SRC} "\n")
    endforeach()
endfunction(check_includes)

########################################
# FUNCTION check_functions
########################################
function(check_functions functions)
    foreach(F ${${functions}})
	set(name ${F})
	string(TOUPPER ${name} name)
	check_function_exists(${F} HAVE_${name})
	file(APPEND ${AUTOCONFIG_SRC} "/* Define to 1 if you have the `${F}' function. */\n")
	file(APPEND ${AUTOCONFIG_SRC} "#cmakedefine HAVE_${name} 1\n")
	file(APPEND ${AUTOCONFIG_SRC} "\n")
    endforeach()
endfunction(check_functions)

########################################

file(WRITE ${AUTOCONFIG_SRC})

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckCXXSourceCompiles)
include(CheckCXXSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckPrototypeDefinition)
include(CheckStructHasMember)
include(CheckSymbolExists)
include(CheckTypeSize)

set(include_files_list
    ctype.h
    dlfcn.h
    errno.h
    float.h
    inttypes.h
    limits.h
    memory.h
    stdint.h
    stdlib.h
    stdio.h
    strings.h
    string.h
    sys/stat.h
    sys/time.h
    sys/types.h
    time.h
    unistd.h
)
check_includes(include_files_list)

set(functions_list
    gettimeofday
    localtime
    localtime_r
    gmtime
    gmtime_r
    strcasecmp
    stricmp
)
check_functions(functions_list)

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(LEPT lept)
    pkg_check_modules(LUA53 lua5.3)
    pkg_check_modules(SDL2 sdl2)
else()
    if (USE_SDL2)
	include( FindSDL2 )
     endif()
endif()

if (SDL2_FOUND)
    set(HAVE_SDL2 1)
endif()

file(APPEND ${AUTOCONFIG_SRC} "
/* Define to 1 if you have SDL2. */
#cmakedefine HAVE_SDL2 1

/* Name of package */
#define PACKAGE             \"lualept\"

/* Define to the address where bug reports for this package should go. */
#define PACKAGE_BUGREPORT   \"https://github.com/pullmoll/lualept/\"

/* Define to the full name of this package. */
#define PACKAGE_NAME        \"lualept\"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING      \"lualept-${VERSION_PLAIN}\"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME     \"lualept\"

/* Define to the home page for this package. */
#define PACKAGE_URL         \"https://github.com/pullmoll/lualept/\"

/* Define to the version of this package. */
#define PACKAGE_VERSION     \"${VERSION_PLAIN}\"
")

########################################

################################################################################
