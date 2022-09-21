add_library(ignData STATIC
    src/ignDataBoolean.cpp
    src/ignDataInteger.cpp
    src/ignDataLong.cpp
    src/ignDataDouble.cpp
    src/ignDataNull.cpp
    src/ignDataFloat.cpp
    src/ignDataValue.cpp
    src/ignDataHelper.cpp
    src/ignDataNameValuePair.cpp
    src/ignDataString.cpp
    src/ignDataArray.cpp    
    src/ignDataObject.cpp
    src/ignDataJsonLexer.cpp
    src/ignDataJsonReader.cpp
    src/ignDataVariant.cpp
)

target_include_directories(ignData PUBLIC include)
target_link_libraries(ignData PUBLIC ignCore ignNumeric)
target_link_libraries(ignData PUBLIC Boost::boost Boost::filesystem)
