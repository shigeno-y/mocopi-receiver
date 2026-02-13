# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.27)

project(parse LANGUAGES CXX)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_compile_definitions(${PROJECT_NAME}
                           PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>")

find_package(CLI11 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)

target_link_libraries(${PROJECT_NAME} PRIVATE shigenoy::libmocopiReceiver)
