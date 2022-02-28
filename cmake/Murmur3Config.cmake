set(NUKE_EXAMPLES_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
message(STATUS "Found NDK examples: ${NUKE_EXAMPLES_ROOT}")

add_library(Murmur3 SHARED IMPORTED)

if (UNIX)
    set_target_properties(Murmur3
        PROPERTIES
            IMPORTED_LOCATION ${NUKE_EXAMPLES_ROOT}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}Murmur3${CMAKE_SHARED_LIBRARY_SUFFIX}
            INTERFACE_INCLUDE_DIRECTORIES ${NUKE_EXAMPLES_ROOT}/include
            INTERFACE_LINK_LIBRARIES ${NUKE_EXAMPLES_ROOT}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}Murmur3${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
else()
    set_target_properties(Murmur3
        PROPERTIES
            IMPORTED_LOCATION ${NUKE_EXAMPLES_ROOT}/bin/Murmur3.dll
            IMPORTED_IMPLIB ${NUKE_EXAMPLES_ROOT}/lib/Murmur3.lib
            INTERFACE_INCLUDE_DIRECTORIES ${NUKE_EXAMPLES_ROOT}/include
    )
endif()