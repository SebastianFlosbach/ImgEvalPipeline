cmake_minimum_required (VERSION 3.8)

set (Boost_INCLUDE_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/include)
set (Boost_LIBRARY_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/lib)
set (Boost_PROGRAM_OPTIONS_LIBRARY $ENV{VCPKG_ROOT}/installed/x64-windows/lib/boost_program_options-vc140-mt.lib)

find_package (IlmBase CONFIG REQUIRED)
find_package (OpenCV CONFIG REQUIRED core imgcodecs highgui features2d xfeatures2d)
find_package (AliceVision CONFIG REQUIRED)

file(GLOB_RECURSE "SOURCES" "include/*.hpp" "src/*.cpp")
message(STATUS ${SOURCES})

add_executable (OpenCVTestPipeline ${SOURCES})
set_target_properties(OpenCVTestPipeline PROPERTIES CXX_STANDARD 17)
target_include_directories(OpenCVTestPipeline PUBLIC ${Boost_INCLUDE_DIR} "include")
target_link_libraries (OpenCVTestPipeline PUBLIC IlmBase::Iex IlmBase::Half IlmBase::Imath IlmBase::IexMath opencv_core opencv_imgcodecs opencv_highgui opencv_features2d opencv_xfeatures2d aliceVision_sfmDataIO ${Boost_PROGRAM_OPTIONS_LIBRARY})

if($<CONFIG:Debug>)
    set(ALICEVISION_CONFIG Debug)
else()
    set(ALICEVISION_CONFIG Release)
endif()

# Quick hack because install command causes never ending cache generation in Visual Studio 2019
set(ALICEVISION_LIBS sfmData sfmDataIO feature image numeric)
foreach(lib ${ALICEVISION_LIBS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{AliceVision_INSTALL}/${ALICEVISION_CONFIG}/bin/aliceVision_${lib}.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{AliceVision_INSTALL}/${ALICEVISION_CONFIG}/bin/vlsift.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

set(BOOST_LIBS filesystem log thread program_options)
foreach(lib ${BOOST_LIBS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{VCPKG_ROOT}/installed/x64-windows/bin/boost_${lib}-vc142-mt-x64-1_76.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()
