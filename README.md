# Description

The aim of this project is to design a **SEN-15901 emulator**. The program simulates the waveforms of the wind vane and the rain gauge, in order to test the [sen15901-driver](https://github.com/Ludovic-Lesur/sen15901-driver).

# Hardware

The board was designed on **Circuit Maker V2.0**. Below is the list of hardware revisions:

| Hardware revision | Description | `cmake_hw_version` | Status |
|:---:|:---:|:---:|:---:|
| [SEN15901-EMULATOR HW1.0](https://365.altium.com/files/107B18F5-BE93-4D1A-97D6-74040E0FA2C4) | Initial version. | `HW1_0` | :white_check_mark: |

# Embedded software

## Environment

The embedded software is developed under **Eclipse IDE** version 2024-09 (4.33.0) and **GNU MCU** plugin. The `script` folder contains Eclipse run/debug configuration files and **JLink** scripts to flash the MCU.

## Target


The board is based on the **STM32L041K6U6** microcontroller of the STMicroelectronics L0 family. Each hardware revision has a corresponding **build configuration** in the Eclipse project, which sets up the code for the selected board version.

## Structure

The project is organized as follow:

* `drivers` :
    * `device` : MCU **startup** code and **linker** script.
    * `registers` : MCU **registers** address definition.
    * `peripherals` : internal MCU **peripherals** drivers.
    * `components` : external **components** drivers.
    * `utils` : **utility** functions.
* `middleware` :
    * `simulation` : SEN15901 **simulator state machine**.
* `application` : Main **application**.

## Build

The project can be compiled by command line with `cmake`.

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="script/cmake-arm-none-eabi/toolchain.cmake" \
      -DTOOLCHAIN_PATH="<arm_none_eabi_gcc_path>" \
      -DSEN15901_EMULATOR_HW_VERSION="<cmake_hw_version>" \
      -DSEN15901_EMULATOR_MODE_ULTIMETER=OFF \
      -G "Unix Makefiles" ..
make all
```
