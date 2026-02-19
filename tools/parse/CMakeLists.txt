# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.27)

project(parse LANGUAGES CXX)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
target_compile_options(
  ${PROJECT_NAME}
  PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/permissive->" #
          "$<$<CXX_COMPILER_ID:MSVC>:/MP>" #
          "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>" #
          "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>" #
)
target_link_options(
  ${PROJECT_NAME} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/Brepro>" #
)
target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>" #
          "$<$<CXX_COMPILER_ID:MSVC>:_UNICODE>" #
          "$<$<CXX_COMPILER_ID:MSVC>:UNICODE>" #
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

find_package(CLI11 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)

target_link_libraries(${PROJECT_NAME} PRIVATE shigenoy::libmocopiParser)

find_package(pxr CONFIG REQUIRED)
if(MSVC)
  set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
                                               ${CMAKE_CURRENT_BINARY_DIR})
  string(
    JOIN
    "\n"
    VS_DEBUGGER_ENV_VARS
    # for Debug
    "PATH=$<PATH:APPEND,$<PATH:GET_PARENT_PATH,${PXR_INCLUDE_DIRS}>,bin>;$<PATH:APPEND,$<PATH:GET_PARENT_PATH,${PXR_INCLUDE_DIRS}>,lib>;$<PATH:APPEND,$<PATH:GET_PARENT_PATH,${PXR_INCLUDE_DIRS}>,plugin,usd>;$<PATH:APPEND,${CMAKE_INSTALL_PREFIX},bin>;$ENV{PATH}"
    "PYTHONPATH=$<PATH:APPEND,$<PATH:GET_PARENT_PATH,${PXR_INCLUDE_DIRS}>,lib,python>;$ENV{PYTHONPATH}"
    # for ALL
    "SPDLOG_LEVEL=trace,usdMMDPlugins=trace"
    "PXR_PLUGINPATH_NAME=$<LIST:FILTER,$<LIST:REMOVE_DUPLICATES,$<SHELL_PATH:$<PATH:APPEND,${CMAKE_INSTALL_PREFIX},plugin,usd>>;$ENV{PXR_PLUGINPATH_NAME}>,EXCLUDE,^$>"
  )
  set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_ENVIRONMENT
                                               ${VS_DEBUGGER_ENV_VARS})
endif()
