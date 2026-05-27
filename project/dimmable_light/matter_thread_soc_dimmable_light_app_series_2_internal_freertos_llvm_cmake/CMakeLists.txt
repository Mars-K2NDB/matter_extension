# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(matter_thread_soc_dimmable_light_app_series_2_internal_freertos)

include(ExternalProject)

ExternalProject_Add(matter_thread_soc_dimmable_light_app_freertos
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../dimmable_light_app/cmake_llvm
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../dimmable_light_app/cmake_llvm/build
  CMAKE_ARGS --preset project
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
  INSTALL_COMMAND ""
  BUILD_ALWAYS true
  USES_TERMINAL_BUILD 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../dimmable_light_app/cmake_llvm
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

ExternalProject_Add_StepDependencies(matter_bootloader_internal_series_2 build matter_thread_soc_dimmable_light_app_freertos)


set(post_build_command ${POST_BUILD_EXE} postbuild "./matter_thread_soc_dimmable_light_app_series_2_internal_freertos.slpb")


add_custom_target(postbuild ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)

add_dependencies(postbuild matter_thread_soc_dimmable_light_app_freertos matter_bootloader_internal_series_2)

# BEGIN_SIMPLICITY_STUDIO_METADATA=eJylU91r2zAQ/1eMKaN9mJV2b5m3Mpo9GOxmzClsYBCyfIm1yTpPp8yBsf991yaOU7LCYI86/77uw7/iMis+5dldtvoqy9XDIlvKcpk/rLLlvSyWi495PI/T211no5/gyaB7V8XXyayKI3AaG+M2XPhQ3mVZFd++r1xalGi3gYEUMcvRfEeGIW0I/VyIYRiS4U2CfiNuZrNr8aXIS91Cp14bR0E5DazMjLnDe9UB9UrDHpCjVmHvf6JFxqqaEo2dIBKjdQIaPStxnihK6VB9evF7RBXYbC18hjV47gUoMg2Lk9XJgP77k3XSqKCqWBypi2IBa+PMI/9l/AHOhB4p1FtjmzJAfyzzh6l4ccnpO+Ua8FfRER+9+rHF8Pbickx/JToVAngZWg+qkYRaNoaJtQVpzaYNUvW9JPAGSN5I4xjslJVrD+ADUkK2r/eqqTjPlIq/Z017j99Ah8jxPrjff0oxevIy7bQ30WIH3IansRfYBXCPVyUOLuK5mDjXnrbxYrYaMVhUPNBpCuNc/jvRmcfJeYhn97G/PnFyfqmY/g5+xr//AOIWP30==END_SIMPLICITY_STUDIO_METADATA