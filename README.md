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


## Build instructions ##

```
> $ cd audio_dev_kit_bsp/
> $ cmake -DBUILD_ARM=ON -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -S base/src/ -B base/build/
> $ cmake --build base/build/
> $ cmake -S base/src/ -B base/build_host/
> $ cmake --build base/build_host/
```
