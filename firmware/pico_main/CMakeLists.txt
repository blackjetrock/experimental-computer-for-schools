# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_BUILD_TYPE Debug)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH    "/tree/downloaded_tools/pico/pico-sdk")
set(PICO_EXTRAS_PATH "/tree/downloaded_tools/pico/pico-extras")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
project(experimental_schools_computer C CXX ASM)

set(PICO_COPY_TO_RAM 1)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

add_compile_definitions(PICO_STACK_SIZE=10000)
add_compile_definitions(PICO_CORE1_STACK_SIZE=4000)
add_compile_definitions(PICO_USE_STACK_GUARDS=1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# SD FAT LIB
add_subdirectory(/tree/downloaded_tools/pico/ajm-no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

# Add executable. Default name is the project name, version 0.1

add_executable(experimental_schools_computer
  experimental_schools_computer.c
  fsms.c
  esc_fsms.c
  match_token.c
  oled.c
  sdcard.c

  )

#add_library( gcc111libbid_hard UNKNOWN IMPORTED)
#SET_PROPERTY(TARGET gcc111libbid_hard  PROPERTY IMPORTED_LOCATION /tree/projects/github/fx201p-ram-replacement/firmware/experimental_schools_computer/build/gcc111libbid_hard.a)

#pico_generate_pio_header(experimental_schools_computer ${CMAKE_CURRENT_LIST_DIR}/picoputer.pio)

pico_set_program_name(experimental_schools_computer "experimental_schools_computer")
pico_set_program_version(experimental_schools_computer "0.1")

#pico_enable_stdio_uart(experimental_schools_computer 0)
pico_enable_stdio_usb(experimental_schools_computer 1)

# Add the standard library to the build
target_link_libraries(experimental_schools_computer pico_stdlib )

IF (NOT DEFINED N_SD_CARDS)
  SET(N_SD_CARDS 1)
ENDIF()

target_compile_definitions(experimental_schools_computer PUBLIC N_SD_CARDS=${N_SD_CARDS})

# Add any user requested libraries
target_link_libraries(experimental_schools_computer
 #gcc111libbid_hard
        #hardware_i2c
        hardware_pio
        hardware_clocks
	hardware_adc
	hardware_dma
        pico_sd_card
        pico_multicore
	FatFs_SPI
        )

pico_add_extra_outputs(experimental_schools_computer)

