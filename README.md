# Description

The aim of this project is to design a **SEN-15901 emulator**. The program simulates the waveforms of the wind vane and the rain gauge, in order to test the [sen15901-driver](https://github.com/Ludovic-Lesur/sen15901-driver).

# Hardware

The project is currently under development on a **Nucleo-L031K6 board**.

# Embedded software

## Environment

The embedded software is developed under **Eclipse IDE** version 2024-09 (4.33.0) and **GNU MCU** plugin. The `script` folder contains Eclipse run/debug configuration files and **JLink** scripts to flash the MCU.

## Target

The project is currently under development on a **STM32L031K6U6** microcontroller of the STMicroelectronics L0 family.

## Structure

The project is organized as follow:

* `startup` : MCU **startup** code (from ARM).
* `linker` : MCU **linker** script (from ARM).
* `drivers` :
    * `registers` : MCU **registers** address definition.
    * `peripherals` : internal MCU **peripherals** drivers.
    * `components` : external **components** drivers.
    * `utils` : **utility** functions.
* `application` : Main **application**.
