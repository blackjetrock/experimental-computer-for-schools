cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(picocalc_helloworld C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}
)

add_subdirectory(i2ckbd)
add_subdirectory(lcdspi)

add_executable(picocalc_helloworld
	main.c
	)

target_link_libraries(picocalc_helloworld
    pico_stdlib
    hardware_flash
    hardware_irq
    hardware_adc
    hardware_pwm
    hardware_i2c
    hardware_spi
    hardware_dma
    hardware_exception
	hardware_pio
    pico_multicore
	i2ckbd
	lcdspi

	)

pico_enable_stdio_usb(picocalc_helloworld 0)
pico_enable_stdio_uart(picocalc_helloworld 1)

pico_add_extra_outputs(picocalc_helloworld)
