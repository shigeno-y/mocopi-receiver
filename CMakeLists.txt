# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.27)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse HEAD
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse --short HEAD
  OUTPUT_VARIABLE GIT_SHORT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse --abbrev-ref HEAD
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} describe --tags --abbrev=0
  OUTPUT_VARIABLE GIT_TAG_CANDIDATE
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} describe --tags --dirty
  OUTPUT_VARIABLE GIT_TAG_DIRTY_CHECKER
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if(GIT_TAG_CANDIDATE STREQUAL GIT_TAG_DIRTY_CHECKER)
  string(REGEX MATCH "^([0-9]+\.[0-9]+\.[0-9]+)" clean_version
               "${GIT_TAG_CANDIDATE}")
  set(top_project_version "${clean_version}.0")
else()
  string(REGEX MATCH "^([0-9]+\.[0-9]+\.[0-9]+)" clean_version
               "${GIT_TAG_CANDIDATE}")
  execute_process(
    COMMAND git -C ${CMAKE_SOURCE_DIR} rev-list --count HEAD
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(top_project_version "${clean_version}.${GIT_COMMIT_COUNT}")
endif()

project(
  mocopiReceiver
  LANGUAGES CXX
  VERSION ${top_project_version})

add_subdirectory(libmocopiReceiver)

include(CMakePackageConfigHelpers)
install(
  EXPORT "${PROJECT_NAME}Targets"
  NAMESPACE ${PROJECT_NAME}::
  FILE "${PROJECT_NAME}Targets.cmake"
  DESTINATION lib/cmake/${PROJECT_NAME})
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE"
        DESTINATION lib/cmake/${PROJECT_NAME})

# generate the config file that includes the exports
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.in.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
  NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
  COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION lib/cmake/${PROJECT_NAME})

include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)
get_all_targets(targets ${CMAKE_CURRENT_SOURCE_DIR})
foreach(target ${targets})
  get_target_property(current_name ${target} NAME)
  get_target_property(current ${target} SOURCE_DIR)
  if(current)
    cmake_path(GET current FILENAME current_parent)
    if((current_name STREQUAL current) OR (current_name STREQUAL current_parent
                                          ))
      cmake_path(GET current PARENT_PATH current)
    endif()
    message("${current_name}\t${current}\t${current_parent}")
    file(RELATIVE_PATH current ${CMAKE_SOURCE_DIR} ${current})
    set_target_properties(${target} PROPERTIES FOLDER "${current}")
  endif(current)
endforeach(target ${targets})
