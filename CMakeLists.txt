cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

project(PlasickaKun LANGUAGES C CXX ASM)

# CubeMX generates main.c; its user sections contain C++.
set_source_files_properties(Core/Src/main.c PROPERTIES LANGUAGE CXX)
add_executable(${PROJECT_NAME})
add_subdirectory(cmake/stm32cubemx)

# Match the vector table address in the linker script.
target_compile_definitions(STM32_Drivers PRIVATE USER_VECT_TAB_ADDRESS)

target_sources(${PROJECT_NAME} PRIVATE
    Core/App/Src/HBridgePwm.cpp
    Core/App/Src/ChirpGenerator.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE Core/App/Inc)
target_link_libraries(${PROJECT_NAME} stm32cubemx)

# STM32 toolchain workaround for mixed C/C++ linking.
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
