# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.27)

project(libmocopiReceiver LANGUAGES CXX)

add_library(${PROJECT_NAME})
add_library(shigenoy::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME}
  PUBLIC FILE_SET
         headers
         TYPE
         HEADERS
         BASE_DIRS
         include
         FILES
         include/shigenoy/mocopi-receiver/Data.hpp #
         include/shigenoy/mocopi-receiver/Data/Container.hpp #
         include/shigenoy/mocopi-receiver/Data/FRAM.hpp #
         include/shigenoy/mocopi-receiver/Data/HEAD.hpp #
         include/shigenoy/mocopi-receiver/Data/SKDF.hpp #
         include/shigenoy/mocopi-receiver/Data/SNDF.hpp #
         include/shigenoy/mocopi-receiver/Data/SNDT.hpp #
         include/shigenoy/mocopi-receiver/Parser.hpp #
         include/shigenoy/mocopi-receiver/Receiver.hpp #
)
target_sources(
  ${PROJECT_NAME}
  PRIVATE src/Data.cpp #
          src/Data/Container.cpp #
          src/Data/FRAM.cpp #
          src/Data/HEAD.cpp #
          src/Data/SKDF.cpp #
          src/Data/SNDF.cpp #
          src/Data/SNDT.cpp #
          src/Parser.cpp #
          src/Receiver.cpp #
)
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
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF
             DEBUG_POSTFIX d)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT "mocopiReceiverTargets"
  FILE_SET headers
  DESTINATION "include"
  ARCHIVE DESTINATION "lib")

if(MSVC)
  get_target_property(_s ${PROJECT_NAME} HEADER_SET_headers)
  if(_s)
    source_group(
      TREE "${PROJECT_SOURCE_DIR}/include"
      PREFIX "\\Header Files\\include"
      FILES ${_s})
  endif()
  get_target_property(_s ${PROJECT_NAME} HEADER_SET_internalHeader)
  if(_s)
    source_group(
      TREE "${PROJECT_SOURCE_DIR}/internal_include"
      PREFIX "\\Header Files\\internal_include"
      FILES ${_s})
  endif()
  get_target_property(_s ${PROJECT_NAME} SOURCES)
  if(_s)
    source_group(
      TREE "${PROJECT_SOURCE_DIR}/src"
      PREFIX "\\Source Files"
      FILES ${_s})
  endif()
endif()
