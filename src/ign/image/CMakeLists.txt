add_library(ignImage STATIC
   src/ignImageType.cpp
   src/ignImageCache.cpp
   src/ignColorTable.cpp
   src/ignMultiImage.cpp
   src/ignImage.cpp
   src/ignBufferImage.cpp
   src/ignImageDriverManager.cpp
   src/ignImageDriver.cpp
   src/ignMPDImageDriver.cpp
   src/ignImageIO.cpp
   src/ignImageInput.cpp
   src/ignImageOutput.cpp
   src/ignGDALImageDriver.cpp
   src/ignImageDeriche.cpp
   src/ignImageEntropy.cpp
   src/ignImageWatershed.cpp
   src/ignImageSystemRadio.cpp
   src/ignImageAddChannels.cpp
   src/ignImageNumRegion.cpp
   src/ignProjectionList.cpp
)
   
target_include_directories(ignImage PUBLIC include)
target_link_libraries(ignImage  PUBLIC 
    ignCore 
    ignNumeric
    ignData
    ignTransform
)
find_package(GDAL CONFIG REQUIRED)

target_link_libraries(ignImage PUBLIC GDAL::GDAL)
target_link_libraries(ignImage PUBLIC Boost::boost Boost::filesystem)

target_compile_definitions(ignImage PUBLIC IGN_WITH_GDAL)
