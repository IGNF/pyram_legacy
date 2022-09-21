add_library(ignNumeric STATIC 
    src/ignNumeric.cpp
    src/ignNumericTypes.cpp
    src/ignNumericSysUnit.cpp
)
target_include_directories(ignNumeric PUBLIC include)
target_link_libraries(ignNumeric PUBLIC ignCore)
target_link_libraries(ignNumeric PUBLIC Boost::boost Boost::filesystem)