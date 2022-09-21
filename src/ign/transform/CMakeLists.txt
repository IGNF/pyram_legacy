add_library(ignTransform STATIC 
    src/ignTransform.cpp 
    src/ignTransformSystem.cpp 
    src/ignTransformVector.cpp 
)
target_include_directories(ignTransform PUBLIC include)
target_link_libraries(ignTransform PUBLIC ignCore ignNumeric)
target_link_libraries(ignTransform PUBLIC Boost::boost Boost::filesystem)