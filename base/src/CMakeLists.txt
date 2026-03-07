cmake_minimum_required(VERSION 3.21.4)

# Leave as debug for now
set(CMAKE_BUILD_TYPE Debug)

set(TINYUSB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyusb)

FILE(GLOB_RECURSE TinyUSBCSources ${TINYUSB_PATH}/src/*.c)

# PLace your c and c++ files here
set(SOURCES
	${TinyUSBCSources}
	${CMAKE_CURRENT_SOURCE_DIR}/init/startup.c
	${CMAKE_CURRENT_SOURCE_DIR}/system/syscalls.c
	${CMAKE_CURRENT_SOURCE_DIR}/init/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gpio/gpio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/pin_defs.c
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_gpio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/uart/uart.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_uart.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/dma/dma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_dma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/adc/adc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_adc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/spi/spi.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_spi.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/usb/usb_descriptors.c
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_tinyusb.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/timer/basic_timer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_timers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/util.c
	${CMAKE_CURRENT_SOURCE_DIR}/usb/usb_serial.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_usb_serial.cpp
)

# TinyUSB stuff

include(${TINYUSB_PATH}/src/CMakeLists.txt)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/adc
	${CMAKE_CURRENT_SOURCE_DIR}/board
	${CMAKE_CURRENT_SOURCE_DIR}/dma
	${CMAKE_CURRENT_SOURCE_DIR}/gpio
	${CMAKE_CURRENT_SOURCE_DIR}/spi
	${CMAKE_CURRENT_SOURCE_DIR}/system
	${CMAKE_CURRENT_SOURCE_DIR}/tests
	${CMAKE_CURRENT_SOURCE_DIR}/timer
	${CMAKE_CURRENT_SOURCE_DIR}/uart
	${CMAKE_CURRENT_SOURCE_DIR}/usb
	${CMAKE_CURRENT_SOURCE_DIR}/util
	${TINYUSB_PATH}/src	
)

# PROJECT_NAME
project(audio_dev_kit_bsp)

# Option to choose target (default: native)
option(BUILD_ARM "Build for ARM Cortex-M" OFF)

if (BUILD_ARM)

	# Place directories to include here
	include_directories()


	# Create elf file and link it
	add_executable(${PROJECT_NAME}.elf ${SOURCES})
	target_link_libraries(${PROJECT_NAME}.elf)

	# Create binary file
	add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${PROJECT_NAME}.elf)
	add_custom_command(TARGET ${PROJECT_NAME}.bin COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)
    
else()
	add_subdirectory(third_party/googletest)

	# Unit test target (Runs on PC)
	add_executable(my_tests test_test/hello_test.cpp)
	add_executable(dac_tests test_test/dac_test.cpp)

	target_link_libraries(my_tests PRIVATE gtest_main gmock_main gtest)
	target_link_libraries(dac_tests PRIVATE gtest_main gmock_main gtest)

	include(GoogleTest)
	gtest_discover_tests(dac_tests)
	gtest_discover_tests(dac_tests)
endif()


