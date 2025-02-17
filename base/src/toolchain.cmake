# set(BAREMETAL_ARM_TOOLCHAIN_PATH /usr/bin/)

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

if(WIN32)
    set(TOOLCHAIN_EXT ".exe")
else()
    set(TOOLCHAIN_EXT "")
endif(WIN32)

set(TARGET_TRIPLET "arm-none-eabi-")

# Point CMake to all of the tools
# set(CMAKE_AR                        ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-ar${CMAKE_EXECUTABLE_SUFFIX})
# set(CMAKE_ASM_COMPILER              ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
# set(CMAKE_C_COMPILER                ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
# set(CMAKE_CXX_COMPILER              ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-g++${CMAKE_EXECUTABLE_SUFFIX})
# set(CMAKE_LINKER                    ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-ld${CMAKE_EXECUTABLE_SUFFIX})
# set(CMAKE_OBJCOPY                   ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-objcopy${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
# set(CMAKE_RANLIB                    ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-ranlib${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
# set(CMAKE_SIZE                      ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-size${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
# set(CMAKE_STRIP                     ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-strip${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")

set(CMAKE_C_COMPILER   ${TARGET_TRIPLET}gcc${TOOLCHAIN_EXT})
set(CMAKE_CXX_COMPILER ${TARGET_TRIPLET}g++${TOOLCHAIN_EXT})
set(CMAKE_ASM_COMPILER ${TARGET_TRIPLET}gcc${TOOLCHAIN_EXT})
set(CMAKE_LINKER       ${TARGET_TRIPLET}gcc${TOOLCHAIN_EXT})
set(CMAKE_SIZE_UTIL    ${TARGET_TRIPLET}size${TOOLCHAIN_EXT})
set(CMAKE_OBJCOPY      ${TARGET_TRIPLET}objcopy${TOOLCHAIN_EXT})
set(CMAKE_OBJDUMP      ${TARGET_TRIPLET}objdump${TOOLCHAIN_EXT})
set(CMAKE_NM_UTIL      ${TARGET_TRIPLET}gcc-nm${TOOLCHAIN_EXT})
set(CMAKE_AR           ${TARGET_TRIPLET}gcc-ar${TOOLCHAIN_EXT})
set(CMAKE_RANLIB       ${TARGET_TRIPLET}gcc-ranlib${TOOLCHAIN_EXT})

# Flags for your compilers
set(CMAKE_C_FLAGS                   " -mfpu=fpv5-d16 -mfloat-abi=hard --specs=nosys.specs -fdata-sections -ffunction-sections" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS                 "${CMAKE_C_FLAGS} -fno-exceptions -fno-threadsafe-statics " CACHE INTERNAL "")

set(CMAKE_C_FLAGS_DEBUG             "-mcpu=cortex-m7 -mthumb -O0 -g" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE           "-mcpu=cortex-m7 -mthumb -Os -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

set(CMAKE_EXE_LINKER_FLAGS "-march=armv7e-m -mabi=aapcs -Wl,--gc-sections,-T\"${PROJECT_SOURCE_DIR}/init/stm32h750.ld\" -mabi=aapcs -nostartfiles -lgcc")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# CMake complains about these
SET (CMAKE_C_COMPILER_WORKS 1)
SET (CMAKE_CXX_COMPILER_WORKS 1)
