########################################
# FUNCTION add_prog_target
########################################
function(add_prog_target target)
    set                             (${target}_src "${ARGN}")
    add_executable                  (${target} ${${target}_src})
    if (NOT STATIC)
        target_compile_definitions  (${target} PRIVATE -DLIBLUALEPT_IMPORTS)
    endif()
    target_link_libraries           (${target} lualept)
    set_target_properties           (${target} PROPERTIES FOLDER prog)
endfunction(add_prog_target)
########################################

add_prog_target(llua llua.cpp)

set (INSTALL_PROGS llua)

foreach(make_install ${INSTALL_PROGS})
    install(TARGETS ${make_install} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
endforeach()
