cmake_minimum_required(VERSION 3.22)

project(userlib)
add_library(userlib INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(userlib INTERFACE 
	USE_HAL_DRIVER 
	STM32F103xE
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(userlib INTERFACE
    ../../Core/Inc
    ../../Drivers/STM32F1xx_HAL_Driver/Inc
    ../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ../../Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ../../Drivers/CMSIS/Include
    ../../User/Inc
)

target_sources(userlib INTERFACE
    ../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    ../../User/Src/my_dma.c
    ../../User/Src/oled.c
    ../../User/Src/uart.c
    ../../User/Src/font.c
    ../../User/Src/my_i2c.c
    ../../User/Src/delay.c
    ../../User/Src/dht11.c
    ../../User/Src/ds18b20.c
    ../../User/Src/dht22.c
    ../../User/Src/max7219.c
)

target_link_directories(userlib INTERFACE
)

target_link_libraries(userlib INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


