cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(remote_sensor C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
add_executable(remote_sensor
  error.h
  blink.h
  blink.c
  admin_console.c
  remote_sensor.c
)

pico_add_extra_outputs(remote_sensor)

target_include_directories(remote_sensor PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(
  remote_sensor
  pico_cyw43_arch_lwip_threadsafe_background
  pico_stdlib)