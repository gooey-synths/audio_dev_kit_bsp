cmake_minimum_required(VERSION 3.21.4)

# Set toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake)

# Leave as debug for now
set(CMAKE_BUILD_TYPE Debug)

# PLace your c and c++ files here
set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/init/startup.c
	${CMAKE_CURRENT_SOURCE_DIR}/init/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gpio/gpio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/pin_defs.c
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_gpio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/uart/uart.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_uart.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dma/dma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_dma.cpp

)

# Place directories to include here
include_directories()

# PROJECT_NAME
project(audio_dev_kit_bsp)

# Create elf file and link it
add_executable(${PROJECT_NAME}.elf ${SOURCES})
target_link_libraries(${PROJECT_NAME}.elf)

# Create binary file
add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET ${PROJECT_NAME}.bin
	COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)

add_subdirectory(thirdparty/googletest)

enable_testing()

add_executable(test_test test_test/hello_test.cpp)

target_link_libraries(test_test gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(test_test)
