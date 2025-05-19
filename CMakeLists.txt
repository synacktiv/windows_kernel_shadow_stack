cmake_minimum_required(VERSION 3.10)

project(shadow_stack LANGUAGES C ASM)

enable_language(ASM_MASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(WDK REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${ARCH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${ARCH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${ARCH})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")

set(SIGNTOOL_PATH ${WDK_ROOT}/bin/${WDK_VERSION}/x64/signtool.exe)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
include_directories(src)

wdk_add_driver(
  shadow_stack_driver
  # [WINVER <winver_version>]
  NTDDI_VERSION 0x0A00000E

  ${CMAKE_SOURCE_DIR}/src/driver.c
  ${CMAKE_SOURCE_DIR}/src/driver/ioctl.c
  ${CMAKE_SOURCE_DIR}/src/driver/utils.c
  )

target_link_options(
  shadow_stack_driver
  PRIVATE

  "/CETCOMPAT"
)

add_custom_command(
  TARGET shadow_stack_driver
  POST_BUILD

  COMMAND ${SIGNTOOL_PATH} sign /fd SHA256 $<TARGET_FILE:shadow_stack_driver>
)

add_executable(
  shadow_stack_client

  ${CMAKE_SOURCE_DIR}/src/client.c

  ${CMAKE_SOURCE_DIR}/src/client/utils.c
)

target_link_options(
  shadow_stack_client
  PRIVATE

  "/CETCOMPAT"
)

add_library(
  asm_target
  STATIC

  ${CMAKE_SOURCE_DIR}/src/driver/utils.asm
)

target_compile_options(
  shadow_stack_driver
  PRIVATE

  "/Od"
)

target_link_libraries(
  shadow_stack_driver

  asm_target
)
