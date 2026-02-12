@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

if(BUILD_SHARED_LIBS)
  include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@DynamicTargets.cmake")
else()
  include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@Targets.cmake")
endif()

check_required_components(@PROJECT_NAME@)
