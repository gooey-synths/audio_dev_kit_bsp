target extended-remote /dev/ttyBmpGdb
monitor jtag
attach 1
load base/build/audio_dev_kit_bsp.elf
file base/build/audio_dev_kit_bsp.elf
set mem inaccessible-by-default off
b main