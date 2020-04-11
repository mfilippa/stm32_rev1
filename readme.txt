# STM32 project

Cross platform project that supports:

- STM32F415R8T6 (custom board)
- STM32F103C8T6 (blue pill)
- Windows/Linux simulation
- Matlab simulation

Toolchains for STM32:
- Keil: demo version, 32K limit
  > /env/keil
- arm gcc: free, no limits
  > /env/gcc_arm

Toolchains for other platforms:
- gcc: for linux/windows simulation, using gcc/mingw
  > /env/gcc
- matlab: to simulate in matlab/simulink, using mingw
  > /env/matlab

# Architecture

/env        : contains all build files for all platforms: gcc, arm, matlab, etc
  /gcc      : build files for windows/linux
  /gcc_arm  : build files for arm
  /keil     : keil IDE
  /matlab   : build files and model to run in matlab
/ source    : source code
  /app      : application (cross platform code)
  /lib      : libraries (cross platform code)
  /hal      : hardware abtraction layer (only .h files here)
  /hil      : platform specific hardware implementation

# Platform usage

## gcc compiler - windows/linux

How to set up:
- update main.c in hil
- update all required simulated hardware in hil
- update makefile
- make and run

## Keil compiler 

How to set up:
- open project in env/keil
- options for target > select target device
- project > manage > project items > add all files to compile
- options for target > debug > set target debugger
- ... verify that connects (sw device should populate)
- ... flash download > check program / verify / run
- compile, flash and run

## Arm gcc compiler

*** Current issues ***
- stack is placed at top of RAM (per linker file), set RAM size correctly or micro will crash!
- LTO works but must comment weak IRQ handlers in startup for every used IRQ. LTO seems to miss the weak definition and LTO replaces IRQ functions with loop. Possible bug in tool, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83967

How to set up:
- Get toolchain
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
- Extract all files, add to path

Modifications:
- ldscript:
  > pick target script (only diff is ROM and RAM origin and size)
- startup:
  > pick correct startup file: M0. M3, M4, M7
  > modify Stack_Size if needed
  > check interrupts at __isr_vector and weak handler definitions (def_irq_handler) - see issue with LTO!
- makefile:
  > update ldscript
  > update core
  > set optimizer level and LTO
  > update all files
- compile

Flashing:
- Using STLink utility, set start to 0x0800000, flash binary (bin) and run

Where files come from:
- From /share/gcc-arm-none-eabi/samples, extracted to project the following files:
  > ldscripts/*  - using nokeep.ld (minimum size binary)
  > startup/*
  > makefile