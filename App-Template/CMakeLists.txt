cmake_minimum_required(VERSION 3.14)


# Set flags and directory variables
set(FREERTOS_CFG_DIRECTORY "${CMAKE_SOURCE_DIR}/freertos_config")

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(PicoFreeRTOS C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Pull in FreeRTOS
if ("${PICO_PLATFORM}" STREQUAL "rp2040")
    include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
elseif("${PICO_PLATFORM}" STREQUAL "rp2350-arm-s")
    include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2350_ARM_NTZ/FreeRTOS_Kernel_import.cmake)
elseif("${PICO_PLATFORM}" STREQUAL "rp2350-riscv")
    include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2350_RISC-V/FreeRTOS_Kernel_import.cmake)
endif()

# Add FreeRTOS kernel source as a static library
# add_library(FreeRTOS STATIC
#     ${FREERTOS_KERNEL_PATH}/event_groups.c
#     ${FREERTOS_KERNEL_PATH}/list.c
#     ${FREERTOS_KERNEL_PATH}/queue.c
#     ${FREERTOS_KERNEL_PATH}/stream_buffer.c
#     ${FREERTOS_KERNEL_PATH}/tasks.c
#     ${FREERTOS_KERNEL_PATH}/timers.c
#     ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_3.c
#     ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0/port.c
# )

# Build FreeRTOS
#  target_include_directories(FreeRTOS PUBLIC
#      ${FREERTOS_CFG_DIRECTORY}/
#      ${FREERTOS_KERNEL_PATH}/include
#      ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0
#  )

# Application metadata and defines
add_compile_definitions(APP_NAME="App-Template")
add_compile_definitions(APP_VERSION="1.0.0")
add_compile_definitions(BUILD_NUM=1)
add_compile_definitions(PICO_MAX_IRQ_CHAINED=8)

# Define the main application executable
add_executable(my_app
    src/main.c
    src/my_debug.c
    src/pico_error_str.c
)

# Compile GATT file to header
pico_btstack_make_gatt_header(my_app PRIVATE "${CMAKE_CURRENT_LIST_DIR}/bt_config/led_control.gatt")


target_sources(my_app PRIVATE
  src/freertos_callbacks.c
)

target_include_directories(my_app PRIVATE
    ${FREERTOS_CFG_DIRECTORY}/
    src
    lwip_config
    bt_config
)

target_link_libraries(my_app
    pico_stdlib
    pico_sync
    pico_multicore
    pico_aon_timer

    hardware_adc
    hardware_clocks
    hardware_dma
    hardware_pio
    hardware_spi
    hardware_timer
    hardware_i2c
    # hardware_rtc

    cmsis_core
    FreeRTOS-Kernel-Heap4

    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none # for no wifi lwip is overkill -
    # we might still have to have some architecture for the 
    # cyw43 network chip though. 
    # pico_cyw43_arch_lwip_sys_freertos
    # pico_lwip_http

    # pico_btstack_bnep_lwip_sys_freertos 
    # Adds Bluetooth Network Encapsulation Protocol (BNEP) support using
    # LwIP with FreeRTOS for NO_SYS=0 
)

# either use environment variables or 
# compiler arguments to load ssid and pass
if(NOT DEFINED WIFI_SSID)
    set(WIFI_SSID $ENV{WIFI_SSID})
endif()

if(NOT DEFINED WIFI_PASSWORD)
    set(WIFI_PASSWORD $ENV{WIFI_PASSWORD})
endif()

# Then add them as preprocessor definitions
target_compile_definitions(my_app PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"

    CONFIG_USE_LWIP=1
    USE_PRINTF
    USE_DBG_PRINTF

    CYW43_ENABLE_BLUETOOTH=1
    HAVE_FREERTOS_TASK_NOTIFICATIONS=1 # if we want to use task notifications. 
    # USE_SPI_INTERFACE
)

# STDIO over USB and UART
pico_enable_stdio_usb(my_app 1)
pico_enable_stdio_uart(my_app 1)

# Create UF2 and other outputs
pico_add_extra_outputs(my_app)
