# thanks to:
# https://stackoverflow.com/questions/37434946/how-do-i-iterate-over-all-cmake-targets-programmatically
function(get_all_targets var base_dir)
  set(targets)
  get_all_targets_recursive(targets ${base_dir})
  set(${var}
      ${targets}
      PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
  get_property(
    subdirectories
    DIRECTORY ${dir}
    PROPERTY SUBDIRECTORIES)

  foreach(subdir ${subdirectories})
    get_all_targets_recursive(${targets} ${subdir})
  endforeach()

  get_property(
    current_targets
    DIRECTORY ${dir}
    PROPERTY BUILDSYSTEM_TARGETS)
  list(APPEND ${targets} ${current_targets})
endmacro()
