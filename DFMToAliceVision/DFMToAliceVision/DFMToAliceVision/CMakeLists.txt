﻿cmake_minimum_required (VERSION 3.8)

#if(CMAKE_BUILD_TYPE MATCHES Debug)
#    set (Boost_INCLUDE_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/include)
#    set (Boost_LIBRARY_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/lib)
#    set (Boost_PROGRAM_OPTIONS_LIBRARY $ENV{VCPKG_ROOT}/installed/x64-windows/lib/boost_program_options-vc140-mt.lib)
#else()
#    set (Boost_INCLUDE_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/include)
#    set (Boost_LIBRARY_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib)
#    set (Boost_PROGRAM_OPTIONS_LIBRARY $ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib/boost_program_options-vc140-mt-gd.lib)
#endif()


find_package (IlmBase CONFIG REQUIRED)
find_package (AliceVision CONFIG REQUIRED)
find_package (OpenCV CONFIG REQUIRED highgui)

file(GLOB_RECURSE "SOURCES" "include/*.hpp" "src/*.cpp")

add_executable (DFMToAliceVision ${SOURCES})
set_target_properties(DFMToAliceVision PROPERTIES CXX_STANDARD 17)
#target_include_directories(DFMToAliceVision PUBLIC ${Boost_INCLUDE_DIR} "include")
target_include_directories(DFMToAliceVision PUBLIC "include")
target_link_libraries (DFMToAliceVision PUBLIC 
    IlmBase::Iex 
    IlmBase::Half 
    IlmBase::Imath 
    IlmBase::IexMath 
    aliceVision_sfmDataIO 
    opencv_highgui
    #${Boost_PROGRAM_OPTIONS_LIBRARY}
)

set(ALICEVISION_LIBS sfmData sfmDataIO feature image numeric)
foreach(lib ${ALICEVISION_LIBS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{AliceVision_INSTALL}/bin/aliceVision_${lib}.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{AliceVision_INSTALL}/bin/vlsift.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

install(TARGETS DFMToAliceVision
    RUNTIME DESTINATION bin
)
