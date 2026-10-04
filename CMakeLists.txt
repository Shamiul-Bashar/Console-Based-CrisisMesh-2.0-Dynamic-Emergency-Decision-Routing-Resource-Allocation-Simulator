cmake_minimum_required(VERSION 3.16)
project(CrisisMeshConsole VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(crisismesh_console src/main.cpp)
target_include_directories(crisismesh_console PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(crisismesh_console PRIVATE /W4 /permissive-)
else()
    target_compile_options(crisismesh_console PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(dsa_smoke tests/dsa_smoke.cpp)
    target_include_directories(dsa_smoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME dsa_smoke COMMAND dsa_smoke)
endif()
