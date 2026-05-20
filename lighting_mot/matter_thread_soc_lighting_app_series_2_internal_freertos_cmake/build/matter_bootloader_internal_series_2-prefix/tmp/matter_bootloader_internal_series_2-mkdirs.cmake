# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/../matter_bootloader/cmake_gcc")
  file(MAKE_DIRECTORY "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/../matter_bootloader/cmake_gcc")
endif()
file(MAKE_DIRECTORY
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/../matter_bootloader/cmake_gcc/build"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_bootloader_internal_series_2-prefix"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_bootloader_internal_series_2-prefix/tmp"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_bootloader_internal_series_2-prefix/src/matter_bootloader_internal_series_2-stamp"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_bootloader_internal_series_2-prefix/src"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_bootloader_internal_series_2-prefix/src/matter_bootloader_internal_series_2-stamp"
)

set(configSubDirs base)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_bootloader_internal_series_2-prefix/src/matter_bootloader_internal_series_2-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_bootloader_internal_series_2-prefix/src/matter_bootloader_internal_series_2-stamp${cfgdir}") # cfgdir has leading slash
endif()
