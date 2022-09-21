add_library(lgl STATIC 
     src/lglGeneral.cpp 
     src/lglType.cpp
     src/lglRandom.cpp
     src/lglPalette.cpp
     src/lglNeighborhood.cpp 
     src/lglInterPixelBoundaryTracker.cpp
     src/lglImageManip.cpp
     src/lglAnalyzeHeader.cpp
     src/lglImageIO.cpp)
target_include_directories(lgl PUBLIC include)
target_compile_definitions(lgl PUBLIC LGL_WITH_BOOST)
target_link_libraries(lgl  PUBLIC ignImage)