# CMakeList.txt : CMake project for AlembicReader, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (Boost_INCLUDE_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/include)
set (Boost_LIBRARY_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/lib)
set (Boost_PROGRAM_OPTIONS_LIBRARY $ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib/boost_program_options-vc140-mt-gd.lib)

find_package(Alembic REQUIRED)

# Add source to this project's executable.
add_executable (AlembicReader "AlembicReader.cpp" "AlembicReader.h")
target_include_directories(AlembicReader PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(AlembicReader PUBLIC Alembic::Alembic ${Boost_PROGRAM_OPTIONS_LIBRARY})

set(BOOST_LIBS program_options)
foreach(lib ${BOOST_LIBS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $ENV{VCPKG_ROOT}/installed/x64-windows/bin/boost_${lib}-vc142-mt-x64-1_76.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()
