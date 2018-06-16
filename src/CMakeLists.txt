#
# lualept
#

################################################################################

########################################
# SHARED LIBRARY lualept
########################################

file(GLOB hdr "*.h")
file(GLOB src "*.cpp")

add_library                     (lualept ${LIBRARY_TYPE} ${src} ${hdr})
set_target_properties           (lualept PROPERTIES VERSION ${VERSION_PLAIN})
set_target_properties           (lualept PROPERTIES SOVERSION 1.0 )
if (WIN32)
set_target_properties           (lualept PROPERTIES OUTPUT_NAME       lualept-${VERSION_PLAIN})
set_target_properties           (lualept PROPERTIES DEBUG_OUTPUT_NAME lualept-${VERSION_PLAIN}d)
else()
set_target_properties           (lualept PROPERTIES OUTPUT_NAME       lualept)
set_target_properties           (lualept PROPERTIES DEBUG_OUTPUT_NAME lualept)
endif()

if (NOT STATIC)
    target_compile_definitions  (lualept PRIVATE -DLUALEPT_LIB)
endif()

if (LEPT_FOUND)
    target_include_directories  (lualept PUBLIC ${LEPT_INCLUDE_DIRS})
    target_link_libraries       (lualept ${LEPT_LIBRARIES})
endif()

if (LUA53_FOUND)
    target_include_directories  (lualept PUBLIC ${LUA53_INCLUDE_DIRS})
    target_link_libraries       (lualept ${LUA53_LIBRARIES})
endif()

if (SDL2_FOUND)
    target_include_directories  (lualept PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries       (lualept ${SDL2_LIBRARIES})
endif()

if (UNIX)
    target_link_libraries       (lualept)
endif()

export(TARGETS lualept FILE ${CMAKE_BINARY_DIR}/LuaLeptTagets.cmake)

install(TARGETS lualept EXPORT LuaLeptTargets RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(EXPORT LuaLeptTargets DESTINATION cmake)
install(FILES
    ${hdr}
    ${CMAKE_SOURCE_DIR}/src/lualept.h
    ${CMAKE_SOURCE_DIR}/src/llenviron.h
    DESTINATION include/lualept)

################################################################################
