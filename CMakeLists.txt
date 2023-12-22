cmake_minimum_required(VERSION 3.15)

project(rasterizer)

set(CMAKE_CXX_STANDARD 23)
set(LIBRARY_OUTPUT_PATH 
    ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
    ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(-Wall -Werror)
add_compile_options(-fdiagnostics-color=always -fansi-escape-codes)
add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")
add_compile_definitions(DEBUG)

include_directories(dependencies)
include_directories(src)

find_package(OpenMP REQUIRED)

file(GLOB SRC_LIST src/*.cpp)

add_executable(rasterizer)

target_sources(rasterizer 
PUBLIC
    ${SRC_LIST}
)

target_link_libraries(rasterizer
    ${CMAKE_SOURCE_DIR}/dependencies/glad/glad.lib
    glfw3
    OpenMP::OpenMP_CXX
)
