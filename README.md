![](https://avatars.githubusercontent.com/u/115755700?s=400&u=c9bb15abdc6dfc7dbe06a5b1952d9360dfd12abe&v=4])

# Audio Dev Kit Board Support Package #

This is the board support package for Gooey Synth's Audo Dev Kit.

## Tools ##

### Target MCU: [STM32H750](https://www.st.com/en/microcontrollers-microprocessors/stm32h750-value-line.html)  ###

[Datasheet](https://www.st.com/resource/en/datasheet/stm32h750ib.pdf)


[Reference manual](https://www.st.com/resource/en/reference_manual/rm0433-stm32h742-stm32h743753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)


### Toolchain: [ARM GNU Toolchain](https://developer.arm.com/downloads/-/gnu-rm)

### Build system: [CMake](https://cmake.org) ###

### Debugger: [Black Magic Probe](https://1bitsquared.com/products/black-magic-probe) ###

[Black Magic Probe on a bluepill](https://github.com/koendv/blackmagic-bluepill)

## Directory structure ##
Third party libraries are kept in base/src/thirdparty. They are managed as git submodules.

## Project initialization ##
```
git clone https://github.com/gooey-synths/audio_dev_kit_bsp.git
cd audio_dev_kit_bsp/
git submodule update --init --recursive
```

## Linting ##
Use clang-format as the linting tool. It is currently built into the pipeline. The base style is set to llvm

## Build instructions ##
There are two different build types for this project. The first build type is meant for deploying new firmware to the phsical board. Creating build artifacts via the ARM cross-compiler should be run as follows:

```
> $ cd audio_dev_kit_bsp/
> $ cmake -DBUILD_ARM=ON -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -S base/src/ -B base/build/
> $ cmake --build base/build/
```

The second build type is meant for running on a host machine (such as ubuntu). This build process uses the normal c++ compiler so that googletest can be built into the project and the unit tests can be run. To create the build run the following:
```
> $ cd audio_dev_kit_bsp/
> $ cmake -S base/src/ -B base/build_host/
> $ cmake --build base/build_host/
```

## Unit test instructions ##
Unit tests are run using the Googletest library. They are included in the project as a git submodule. The above build specifies two builds. The first is for building and deploying the firmware to the physical board. The second is for building and running unit tests on a host machine. Run the second set of build commands(build_host) from the build instructions, then: 

```
> $ ./base/build_host/my_tests
```
