# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(matter_thread_soc_extended_color_light_strip_app_series_2_internal_freertos)

include(ExternalProject)

ExternalProject_Add(matter_thread_soc_extended_color_light_strip_app_freertos
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../extended_color_light_strip_app/cmake_iar
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../extended_color_light_strip_app/cmake_iar/build
  CMAKE_ARGS --preset project
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
  INSTALL_COMMAND ""
  BUILD_ALWAYS true
  USES_TERMINAL_BUILD 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../extended_color_light_strip_app/cmake_iar
)
ExternalProject_Add(matter_bootloader_internal_series_2
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../matter_bootloader/cmake_iar
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../matter_bootloader/cmake_iar/build
  CMAKE_ARGS --preset project
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
  INSTALL_COMMAND ""
  BUILD_ALWAYS true
  USES_TERMINAL_BUILD 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../matter_bootloader/cmake_iar
)

ExternalProject_Add_StepDependencies(matter_bootloader_internal_series_2 build matter_thread_soc_extended_color_light_strip_app_freertos)


set(post_build_command ${POST_BUILD_EXE} postbuild "./matter_thread_soc_extended_color_light_strip_app_series_2_internal_freertos.slpb")


add_custom_target(postbuild ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)

add_dependencies(postbuild matter_thread_soc_extended_color_light_strip_app_freertos matter_bootloader_internal_series_2)

# BEGIN_SIMPLICITY_STUDIO_METADATA=eJytU92L00AQ/1dCOOTuwWzvfKvRQ64+BJKrmB4oBJbtZtqsbnbiztYUxP/d6UeayllB8HFnf1+zM/sjLrPiQ549ZIvPslw8zbK5LOf50yKbP8piPnufx9M4vd+2NvoOngy6N1V8m0yqOAKnsTZuzYV35UOWVfH928qlRYl2ExhIEbMcTbdkGNKE0E2F6Ps+6V8l6NfibjK5FZ+KvNQNtOqlcRSU08DKzJg6fFQtUKc0HAA5ahUO/mdaZKxaUqKxFURisE5Ao2clzhNFKR2r+xOfB1SB9cbCR1iB516AIlOzOFmd9Oi/7q2TWgVVxeJEnRUzWBlndvzL+COcCR1SWG6MrcsA3anMF2Px6prTt8rV4G+iEz568W2D4fXV9ZD+RrQqBPAyNB5ULQm1hG0A5tVSo0UvrVk3QVLwppOq6ySBN0DyThrHRKesXHkAH5ASst3y4JCK5/lS8efcaefxC+gQOZ4N9/7PiQZ/HrId5ykabIHb8zT0uJfZbZs4OorLwn+52nmO07uYf4kYLCoewPhSw9v916TP/M5WS/y2W4fNFWerm4rxZ/Ex/vkLhmFZNg===END_SIMPLICITY_STUDIO_METADATA