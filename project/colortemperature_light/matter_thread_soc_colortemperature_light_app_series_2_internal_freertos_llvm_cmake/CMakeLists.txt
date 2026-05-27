# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(matter_thread_soc_colortemperature_light_app_series_2_internal_freertos)

include(ExternalProject)

ExternalProject_Add(matter_thread_soc_colortemperature_light_app_freertos
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../colortemperature_light_app/cmake_llvm
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../colortemperature_light_app/cmake_llvm/build
  CMAKE_ARGS --preset project
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
  INSTALL_COMMAND ""
  BUILD_ALWAYS true
  USES_TERMINAL_BUILD 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../colortemperature_light_app/cmake_llvm
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

ExternalProject_Add_StepDependencies(matter_bootloader_internal_series_2 build matter_thread_soc_colortemperature_light_app_freertos)


set(post_build_command ${POST_BUILD_EXE} postbuild "./matter_thread_soc_colortemperature_light_app_series_2_internal_freertos.slpb")


add_custom_target(postbuild ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)

add_dependencies(postbuild matter_thread_soc_colortemperature_light_app_freertos matter_bootloader_internal_series_2)

# BEGIN_SIMPLICITY_STUDIO_METADATA=eJytU0tr20AQ/itChJIcqnXSm6smlLgHgRWXyoEWBMt6Nba2Xe2oO6vKUPrfO44tyyHxodDjjr7XPPQ7LrL88zy7z5bfZLF8nGULWSzmj8ts8SDzxezTPJ7G6d22sdEv8GTQfSjj62RSxhE4jZVxGy58LO6zrIzvbkuX5gXaLjCQImY5mm7JMKQOoZ0K0fd90r9L0G/EzWRyLb7m80LX0Ki3xlFQTgMrM2Pq8EE1QK3SsAfMUauw9z/RImPVihKNjSASg3UCGj0rcZ4oSulQfXrxe0DlWHUWvsAaPPcCFJmKxcnqpEf/48k6qVRQZSyO1Fk+g7VxZsc/jz/AmdAihVVnbFUEaI9l/jAWLy45faNcBf4qOuKjNz87DO8vLof0V6JRIYCXofagKkmopUaLPkDTgleh8yCt2dRBqraVBN4AyRtpHJOcsnLtAXxASsi2q716Kl5mS8XrmdPW43fQIXK8F+77n9IM3rxcO+5R1NgAt+Vp6A22AdzuysTBTbwueqa88xq3dTbzCjFYVDzwcTrDvP5bwhdeJ2cknt3R/krFyZmmYvyL+Bn/+QtsL1FX=END_SIMPLICITY_STUDIO_METADATA