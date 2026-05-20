# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/../lighting_app/cmake_gcc")
  file(MAKE_DIRECTORY "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/../lighting_app/cmake_gcc")
endif()
file(MAKE_DIRECTORY
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/../lighting_app/cmake_gcc/build"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_thread_soc_lighting_app_freertos-prefix"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_thread_soc_lighting_app_freertos-prefix/tmp"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_thread_soc_lighting_app_freertos-prefix/src/matter_thread_soc_lighting_app_freertos-stamp"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_thread_soc_lighting_app_freertos-prefix/src"
  "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_thread_soc_lighting_app_freertos-prefix/src/matter_thread_soc_lighting_app_freertos-stamp"
)

set(configSubDirs base)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_thread_soc_lighting_app_freertos-prefix/src/matter_thread_soc_lighting_app_freertos-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/mars/matter_extension/lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos_cmake/build/matter_thread_soc_lighting_app_freertos-prefix/src/matter_thread_soc_lighting_app_freertos-stamp${cfgdir}") # cfgdir has leading slash
endif()
