cmake_minimum_required(VERSION 3.10)

project(PyramLegacy
  VERSION
    1.0
  DESCRIPTION
    "Legacy version fo Pyram an image segmentation tool by IGN/Laurent Guigue"
  LANGUAGES
    CXX
)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

find_package(Boost REQUIRED COMPONENTS filesystem regex)

if(NOT TARGET Boost::filesystem)
    add_library(Boost::filesystem IMPORTED INTERFACE)
    set_property(TARGET Boost::filesystem PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
    set_property(TARGET Boost::filesystem PROPERTY
        INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
endif()

if(NOT TARGET Boost::regex)
    add_library(Boost::regex IMPORTED INTERFACE)
    set_property(TARGET Boost::regex PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
    set_property(TARGET Boost::regex PROPERTY
        INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
endif()

add_subdirectory("src")
