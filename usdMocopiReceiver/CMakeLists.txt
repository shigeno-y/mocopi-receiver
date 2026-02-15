# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.27)

project(usdMocopiReceiver LANGUAGES CXX)

set(prefix "")
set(suffix ${CMAKE_SHARED_LIBRARY_SUFFIX})

configure_file("${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.json"
               "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.json.tmp" @ONLY)
file(
  GENERATE
  OUTPUT
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}/resources/plugInfo.json
  INPUT "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.json.tmp"
  NO_SOURCE_PERMISSIONS NEWLINE_STYLE LF)

file(
  GENERATE
  OUTPUT $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugInfo.json
  CONTENT {"Includes":["*/resources/"]} NEWLINE_STYLE LF)

add_library(${PROJECT_NAME} SHARED)
target_sources(
  ${PROJECT_NAME}
  PRIVATE #
          include/api.h #
          include/receiver.hpp #
          include/tokens.hpp #
          src/receiver.cpp #
          src/tokens.cpp #
)
target_include_directories(
  ${PROJECT_NAME}
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
if(MSVC)
  target_sources(${PROJECT_NAME} PRIVATE utf8.manifest)
endif()
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

find_package(sockpp REQUIRED)
if(BUILD_SHARED_LIBS)
  target_link_libraries(${PROJECT_NAME} PRIVATE Sockpp::sockpp)
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE Sockpp::sockpp-static)
endif()

find_package(pxr CONFIG REQUIRED)
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC tf
         gf
         sdf
         usd
         usdGeom
         ar
         arch
         js
         vt)

target_link_libraries(${PROJECT_NAME} PRIVATE shigenoy::libmocopiParser)

if(MSVC)
  set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_COMMAND
                                               "$ENV{ComSpec}")

  set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
                                                      ${PROJECT_NAME})
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

if(APPLE)
  install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION plugin/usd
    ARCHIVE DESTINATION plugin/usd
    LIBRARY DESTINATION plugin/usd
    RESOURCE)
else()
  install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION plugin/usd
    LIBRARY DESTINATION plugin/usd
    RESOURCE)
endif()
install(
  FILES
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}/resources/plugInfo.json
  DESTINATION plugin/usd/${PROJECT_NAME}/resources)
install(FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugInfo.json
        DESTINATION plugin/usd)
