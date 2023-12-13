# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# compile ASM with /usr/bin/arm-none-eabi-gcc
ASM_DEFINES = -DPICO_BOARD=\"pico\" -DPICO_BUILD=1 -DPICO_CORE1_STACK_SIZE=4000 -DPICO_NO_HARDWARE=0 -DPICO_ON_DEVICE=1 -DPICO_STACK_SIZE=10000 -DPICO_USE_STACK_GUARDS=1

ASM_INCLUDES = -I/tree/downloaded_tools/pico/pico-sdk/src/rp2_common/boot_stage2/asminclude -I/tree/downloaded_tools/pico/pico-sdk/src/rp2040/hardware_regs/include -I/tree/downloaded_tools/pico/pico-sdk/src/rp2_common/hardware_base/include -I/tree/downloaded_tools/pico/pico-sdk/src/common/pico_base/include -I/tree/projects/github/experimental-computer-for-schools/firmware/pico_main/build/generated/pico_base -I/tree/downloaded_tools/pico/pico-sdk/src/boards/include -I/tree/downloaded_tools/pico/pico-sdk/src/rp2_common/pico_platform/include -I/tree/downloaded_tools/pico/pico-sdk/src/rp2_common/boot_stage2/include

ASM_FLAGS = -mcpu=cortex-m0plus -mthumb -O3 -DNDEBUG

