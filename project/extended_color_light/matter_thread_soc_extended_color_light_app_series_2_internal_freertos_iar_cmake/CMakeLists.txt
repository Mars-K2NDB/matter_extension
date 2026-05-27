# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(matter_thread_soc_extended_color_light_app_series_2_internal_freertos)

include(ExternalProject)

ExternalProject_Add(matter_thread_soc_extended_color_light_app_freertos
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../extended_color_light_app/cmake_iar
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../extended_color_light_app/cmake_iar/build
  CMAKE_ARGS --preset project
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
  INSTALL_COMMAND ""
  BUILD_ALWAYS true
  USES_TERMINAL_BUILD 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../extended_color_light_app/cmake_iar
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

ExternalProject_Add_StepDependencies(matter_bootloader_internal_series_2 build matter_thread_soc_extended_color_light_app_freertos)


set(post_build_command ${POST_BUILD_EXE} postbuild "./matter_thread_soc_extended_color_light_app_series_2_internal_freertos.slpb")


add_custom_target(postbuild ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)

add_dependencies(postbuild matter_thread_soc_extended_color_light_app_freertos matter_bootloader_internal_series_2)

# BEGIN_SIMPLICITY_STUDIO_METADATA=eJytU0tv00AQ/iuWVaH2gDctt2CoUMPBkt0gnEogWVpt1pN4Yb1jdjY4EuK/M3k4Dmp6QOK44+81D/+Ky6z4lGcP2eKrLBdPs2wuy3n+tMjmj7KYzz7m8TRO77etjX6CJ4PuXRXfJpMqjsBprI1bc+FD+ZBlVXz/vnJpUaLdBAZSxCxH0y0ZhjQhdFMh+r5P+jcJ+rW4m0xuxZciL3UDrXptHAXlNLAyM6YOH1UL1CkNB0COWoWD/5kWGauWlGhsBZEYrBPQ6FmJ80RRSsfq/sXvAVVgvbHwGVbguRegyNQsTlYnPfrve+ukVkFVsThRZ8UMVsaZHf9l/BHOhA4pLDfG1mWA7lTmD2Px6prTt8rV4G+iEz569WOD4e3V9ZD+RrQqBPAyNB5ULQm1hG0A5tVSo0UvrVk3QaqukwTeAMk7aRxTnLJy5QF8QErIdsuDdiqeJ0vF5cRp5/Eb6BA53gp3/Q9ZBmderB13KBpsgVvyNPS1F9hdmDh6iUuSF4s7n3FLL6ZdIgaLigc9zmWY1H9K98zp7HjEX9dzuE1xdpypGP8dfsa//wD+V0xh=END_SIMPLICITY_STUDIO_METADATA