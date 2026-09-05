cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME PlasickaKun)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME} LANGUAGES C CXX ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

# CubeMX keeps main.c, but its user sections contain C++.
# Set this before add_subdirectory so the generated source uses C++ there too.
set_source_files_properties(Core/Src/main.c PROPERTIES LANGUAGE CXX)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Full LL initialization APIs used by HBridgePwm (kept outside generated CMake).
target_compile_definitions(stm32cubemx INTERFACE USE_FULL_LL_DRIVER)
# The linker places the interrupt vector table at FLASH_BASE (0x08000000).
target_compile_definitions(STM32_Drivers PRIVATE USER_VECT_TAB_ADDRESS)
target_sources(STM32_Drivers PRIVATE
    Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.c
    Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.c
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    App/Src/HBridgePwm.cpp
    App/Src/ChirpGenerator.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Core/Inc
    App/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
