# CMakeList.txt : CMake project for AlembicReader, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(Alembic REQUIRED)

# Add source to this project's executable.
add_executable (AlembicReader "AlembicReader.cpp" "AlembicReader.h")
target_link_libraries(AlembicReader Alembic::Alembic)
#target_include_directories(AlembicReader private ${Alembic_INCLUDE_DIR})

# TODO: Add tests and install targets if needed.
