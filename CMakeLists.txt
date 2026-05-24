cmake_minimum_required(VERSION 3.19)
project(plane_intersection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0167 NEW)

find_package(CGAL REQUIRED COMPONENTS Core)

add_executable(intersection src/main.cpp src/MeshLoader.cpp src/Triangulator.cpp src/MeshGraph.cpp)

target_include_directories(intersection PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(intersection PRIVATE CGAL::CGAL)

target_compile_options(intersection PRIVATE -Wall -Wextra -O2)