#
# CMakeLists.txt
#
#  Created on: 14 mar. 2026
#      Author: Ludo
#

# Minimum CMake version.
cmake_minimum_required(VERSION 3.23)

# Project creation.
project(meteofox-sen15901-emulator)
add_executable(${PROJECT_NAME})

# Use object build mode in all sub-directories.
set(BUILD_MODE "OBJECT")

# Macro to convert options to variables.
macro(add_compilation_flag FLAG_NAME FLAG_DESCRIPTION FLAG_DEFAULT_VALUE)
    option(FLAG_NAME FLAG_DESCRIPTION FLAG_DEFAULT_VALUE)
    set(${FLAG_NAME} ${FLAG_DEFAULT_VALUE} CACHE STRING ${FLAG_DESCRIPTION})
    list(APPEND COMPILATION_FLAGS_LIST ${FLAG_NAME})
endmacro()

# Hardware compilation flags.
set(SEN15901_EMULATOR_HW_VERSION "NONE" CACHE STRING "Hardware version")

# Software compilation flags.
add_compilation_flag(SEN15901_MODE_ULTIMETER "Enable Ultimeter wind-vane mode." OFF)

# Hardware specific settings.
# SEN15901_EMULATOR HW1.0.
if(SEN15901_EMULATOR_HW_VERSION STREQUAL "HW1_0")   
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m0plus -O0")
    set(SEN15901_EMULATOR_MCU "stm32l0xx")
    set(PROJECT_LINKER_SCRIPT "stm32l041x6xx.ld")
# Unknown version.
else()
    message(FATAL_ERROR "Invalid hardware version.")
endif()
set(PROJECT_LINKER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/drivers/device/${SEN15901_EMULATOR_MCU}-device/linker")

# Add hardware compilation flags.
add_compile_definitions(
    __SEN15901_EMULATOR_FLAGS_H__
    ${SEN15901_EMULATOR_HW_VERSION}
)

# Add software compilation flags.
foreach(FLAG ${COMPILATION_FLAGS_LIST})
    # Macros only defined.
    if(${FLAG} STREQUAL ON)
        add_compile_definitions(${FLAG})
    endif()
    # Macros with value.
    if((NOT ${FLAG} STREQUAL OFF) AND (NOT ${FLAG} STREQUAL ON))
        set(${FLAG} "(${${FLAG}})")
        add_compile_definitions(${FLAG}=${${FLAG}}) 
    endif()
endforeach()

# Add project sources files.
target_sources(${PROJECT_NAME}
    PRIVATE
        drivers/peripherals/src/mcu_mapping.c
        drivers/components/src/sen15901.c
        drivers/utils/src/terminal_hw.c
        middleware/simulation/src/simulation.c
        application/src/main.c
)

# Project include folders.
include_directories(${PROJECT_NAME}
    PRIVATE
        drivers/device/inc
        drivers/registers/inc
        drivers/registers/${SEN15901_EMULATOR_MCU}-registers/inc
        drivers/peripherals/inc
        drivers/peripherals/${SEN15901_EMULATOR_MCU}-drivers/inc
        drivers/utils/inc
        drivers/utils/embedded-utils/inc
        drivers/components/inc
        middleware/simulation/inc
        application/inc
)

# Add submodules sources files.
add_subdirectory(drivers/device/${SEN15901_EMULATOR_MCU}-device EXCLUDE_FROM_ALL)
add_subdirectory(drivers/peripherals/${SEN15901_EMULATOR_MCU}-drivers EXCLUDE_FROM_ALL)
add_subdirectory(drivers/utils/embedded-utils EXCLUDE_FROM_ALL)

# Link libraries.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${SEN15901_EMULATOR_MCU}-device
        ${SEN15901_EMULATOR_MCU}-drivers
        embedded-utils
        c_nano
        nosys
        gcc
)

# Linker and artifact.
include(script/cmake-arm-none-eabi/linker.cmake)
include(script/cmake-arm-none-eabi/artifact.cmake)
