cmake_minimum_required(VERSION 3.22)

project(user)
add_library(user INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(user INTERFACE 
	USE_HAL_DRIVER 
	STM32F103xE
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(user INTERFACE
    ../../Core/Inc
    ../../Drivers/STM32F1xx_HAL_Driver/Inc
    ../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ../../Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ../../Drivers/CMSIS/Include
)

target_sources(user INTERFACE
    ../../Core/Src/uart.c
)

target_link_directories(user INTERFACE
)

target_link_libraries(user INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


