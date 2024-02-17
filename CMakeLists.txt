cmake_minimum_required(VERSION 3.27)
project(game_jam_02_24)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules/")

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

# Create .h file with source code directory
configure_file(
    ${CMAKE_SOURCE_DIR}/config/source_dir.h.in
    ${CMAKE_SOURCE_DIR}/config/source_dir.h @ONLY
)

include_directories(${CMAKE_SOURCE_DIR}/config)

add_executable(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/main.cpp")

file(GLOB SHADERS "${PROJECT_SOURCE_DIR}/resources/shaders/*")
file(COPY ${SHADERS} DESTINATION "${CMAKE_BINARY_DIR}/shaders")

file(GLOB TEXTURES "${PROJECT_SOURCE_DIR}/resources/textures/*")
file(COPY ${TEXTURES} DESTINATION "${CMAKE_BINARY_DIR}/textures")

# Add other source files to the executable
target_sources(${PROJECT_NAME} PRIVATE ${SHADERS} ${TEXTURES})

set(EXT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/src")
set(EXT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/include")

add_library(glad "${EXT_INCLUDE_DIR}/glad/glad.h" "${EXT_SOURCE_DIR}/glad.c")
target_include_directories(glad PUBLIC ${EXT_INCLUDE_DIR})

include_directories(${EXT_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad PRIVATE ${CMAKE_DL_LIBS})
