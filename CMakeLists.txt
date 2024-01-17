cmake_minimum_required(VERSION 3.9)
project(ImageZoom VERSION 1.0.7 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_BUILD_TYPE "Release")
add_compile_options(/utf-8)

include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/imgui/)

aux_source_directory(${CMAKE_SOURCE_DIR}/src/ SRC_FILES)
aux_source_directory(${CMAKE_SOURCE_DIR}/imgui/ SRC_IMGUI_FILES)

link_directories(${CMAKE_SOURCE_DIR}/lib)

set(SRC  ${SRC_IMGUI_FILES}  ${SRC_FILES})

add_executable(${PROJECT_NAME}   ${SRC} )
target_link_libraries(${PROJECT_NAME}  glfw3)




