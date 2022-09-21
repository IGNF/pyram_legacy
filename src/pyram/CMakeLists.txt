set(HEADER_LIST "exception.h" "pyram.h" "pyram_code.h")

add_executable(pyram pyram.cpp ${HEADER_LIST})
target_include_directories(pyram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# This depends on (header only) boost
target_link_libraries(pyram PRIVATE Boost::boost Boost::filesystem)
target_link_libraries(pyram PRIVATE ignImage sxs)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

#evite un bug de link (fonction identique existant dans la stl et en C pur)
add_definitions(-D_WCHAR_H_CPLUSPLUS_98_CONFORMANCE_)
target_compile_definitions(pyram PRIVATE _WCHAR_H_CPLUSPLUS_98_CONFORMANCE_)

set_target_properties(pyram
    PROPERTIES
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
)

install(TARGETS pyram
        EXPORT PyramTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )