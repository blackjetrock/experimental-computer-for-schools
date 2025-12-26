cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)
set(PICO_SDK_PATH "~/pico/pico-sdk")
set(PICO_EXTRAS_PATH "~/pico-extras")

include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(escw_project C CXX ASM)

set(PICO_COPY_TO_RAM 0)

#set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID for examples")
#set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password for examples")
set(WIFI_SSID "SchoolsCompSim" CACHE INTERNAL "WiFi SSID for examples")
set(WIFI_PASSWORD "schools" CACHE INTERNAL "WiFi password for examples")
add_compile_definitions(ESC_USE_WIFI)
pico_sdk_init()

# SD FAT LIB
add_subdirectory(/tree/downloaded_tools/pico/ajm-no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

add_executable(escw
  ../pico_main/experimental_schools_computer.c
  escw.c
  ../pico_main/fsms.c
  ../pico_main/esc_fsms.c
  ../pico_main/match_token.c
  #picow_access_point.c
  dhcpserver/dhcpserver.c
  dnsserver/dnsserver.c
  ../pico_main/oled.c
  ../pico_main/sdcard.c
  ../pico_main/extracode.c
  ../pico_main/extracode_fp.c
  ../pico_main/extracode_basic.c
  )

#add_executable(escw
#        picow_access_point.c
#        dhcpserver/dhcpserver.c
#        dnsserver/dnsserver.c
#        )

pico_enable_stdio_usb(escw 1)
#pico_enable_stdio_uart(escw 1)
pico_add_extra_outputs(escw)

target_include_directories(escw PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_include_directories(escw PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        ${CMAKE_CURRENT_LIST_DIR}/dnsserver
        )
      
      target_link_libraries(escw
	pico_stdlib
	)

 IF (NOT DEFINED N_SD_CARDS)
  SET(N_SD_CARDS 1)
ENDIF()

target_compile_definitions(escw PUBLIC N_SD_CARDS=${N_SD_CARDS})
target_link_libraries(escw
 #gcc111libbid_hard
        #hardware_i2c
     	pico_cyw43_arch_lwip_threadsafe_background
        pico_multicore
        hardware_pio
        hardware_clocks
	hardware_adc
	hardware_dma
        pico_sd_card
        pico_multicore
	FatFs_SPI
        )
      
pico_add_extra_outputs(escw)

#target_link_libraries(escw
#        pico_cyw43_arch_lwip_threadsafe_background
#        pico_stdlib
#        )


#/ Following from example
#add_executable(picow_access_point_background
#        picow_access_point.c
#        dhcpserver/dhcpserver.c
#        dnsserver/dnsserver.c
#        )



#pico_add_extra_outputs(picow_access_point_background)

#target_include_directories(picow_access_point_poll PRIVATE
#        ${CMAKE_CURRENT_LIST_DIR}
#        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
#        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
#        ${CMAKE_CURRENT_LIST_DIR}/dnsserver
#        )
#target_link_libraries(picow_access_point_poll
#        pico_cyw43_arch_lwip_poll
#        pico_stdlib
#        )
#pico_add_extra_outputs(picow_access_point_poll)
