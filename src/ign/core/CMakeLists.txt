add_library(ignCore STATIC
    src/ignConvertUTF.cpp   
    src/ignStringTools.cpp 
    src/ignAssertion.cpp 
    src/ignLog.cpp 
    src/ignRuntime.cpp 
    src/ignFilesystemSysInfo.cpp 
    src/ignFilesystemUtils.cpp 
)
target_link_libraries(ignCore PUBLIC Boost::boost Boost::filesystem Boost::regex)

target_include_directories(ignCore PUBLIC include)