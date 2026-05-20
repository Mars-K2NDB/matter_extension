# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(matter_thread_soc_lighting_app_series_2_internal_freertos)

include(ExternalProject)

ExternalProject_Add(matter_thread_soc_lighting_app_freertos
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../lighting_app/cmake_llvm
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../lighting_app/cmake_llvm/build
  CMAKE_ARGS --preset project
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
  INSTALL_COMMAND ""
  BUILD_ALWAYS true
  USES_TERMINAL_BUILD 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../lighting_app/cmake_llvm
)
ExternalProject_Add(matter_bootloader_internal_series_2
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../matter_bootloader/cmake_llvm
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../matter_bootloader/cmake_llvm/build
  CMAKE_ARGS --preset project
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
  INSTALL_COMMAND ""
  BUILD_ALWAYS true
  USES_TERMINAL_BUILD 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../matter_bootloader/cmake_llvm
)

ExternalProject_Add_StepDependencies(matter_bootloader_internal_series_2 build matter_thread_soc_lighting_app_freertos)


set(post_build_command ${POST_BUILD_EXE} postbuild "./matter_thread_soc_lighting_app_series_2_internal_freertos.slpb")


add_custom_target(postbuild ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)

add_dependencies(postbuild matter_thread_soc_lighting_app_freertos matter_bootloader_internal_series_2)

# BEGIN_SIMPLICITY_STUDIO_METADATA=eJydUmFr2zAQ/SvClNF+mJV23zJvZTT7YLCbMaewgUEo8iXWJus8nTIHxv57r00dZ3RlsI86vffu3b37lVR5+anIb/LVV1Wt7hb5UlXL4m6VL29VuVx8LJJ5kl3vOyd+QiCL/l2dXKazOhHgDTbWb7nwobrJ8zq5fl/7rKzQ7SIDSTDL03xPliFtjP1cymEY0uFNimErr2azS/mlLCrTQqdfW09RewOszIy5x1vdAfXawAFQoNHx0P9Ei6zTa0oNdpJIjq1TMBhYif0IkdFT9fHF7xFVYrNz8Bk2EHgWIGEbFidn0gHD98fWaaOjrhN5pC7KBWystw/8l/FPcCb0SHG9s66pIvTHMn9MxbNzdt9p30C4EEe8ePVjh/Ht2fno/kJ2OkYIKrYBdKMIjXJ220aOQOm+VwTBAqkrZT3DvHZqEwBCRErJ9euDXiafu8nk311mfcBvYKLwnARP+o/+YzcO0E1ZyRY7YOuBRv+wj+AfLkke6R1Geao1bfxFF2vE6FDz0qZ5xw38p4NnyifByz+SP9yVPDmsTE53z8/k9z26KjDD=END_SIMPLICITY_STUDIO_METADATA