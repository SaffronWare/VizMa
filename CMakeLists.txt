cmake_minimum_required (VERSION 3.8)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("VizMa")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(${PROJECT_SOURCE_DIR}/vendored/SDL EXCLUDE_FROM_ALL)

add_executable(VizMa "Main.cpp")

target_include_directories(VizMa PRIVATE ${PROJECT_SOURCE_DIR}/ArkMat/src)

target_link_libraries(VizMa PRIVATE SDL3::SDL3)