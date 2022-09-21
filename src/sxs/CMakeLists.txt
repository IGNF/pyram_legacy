add_library(sxs STATIC 
     src/sxsImageScaleClimbingUser.cpp 
     src/sxsImageScaleClimbing.cpp 
     src/sxsC_PolygonalBoundary.cpp 
     src/sxsC_PolygonalBoundaryMeasures.cpp 
     src/sxsScaleClimbing.cpp 
     src/sxsScaleSets.cpp 
     src/sxsPiecewiseAffineFunction.cpp
     src/sxsImageScaleSets.cpp

)
target_include_directories(sxs PUBLIC include)
target_link_libraries(sxs  PUBLIC lgl)