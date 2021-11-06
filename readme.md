# STM32 project

Cross platform project that supports:

- STM32F415R8T6 (custom board)
- STM32F103C8T6 (blue pill)
- Windows/Linux simulation
- Matlab simulation

Toolchains:

- /env/gcc_arm  - gcc for arm, free, no limits
- /env/gcc      - for linux/windows simulation, using gcc/mingw64
- /env/matlab   - to simulate in matlab/simulink, using mingw64

# Known issues

- stm32f1 not done or tested. Rest of platforms are working.

# Architecture

    /env        : contains all build files for all platforms: gcc, arm, matlab, etc
      /gcc      : build files for windows/linux
      /gcc_arm  : build files for arm
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

## Arm gcc compiler

### Current issues

- stack is placed at top of RAM (per linker file), set RAM size correctly or micro will crash!
- LTO works but must comment weak IRQ handlers in startup for every used IRQ. LTO seems to miss the weak definition and LTO replaces IRQ functions with loop. Possible bug in tool, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83967

### How to set up:

- Get toolchain

    https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

- Extract all files, add to path

    `tar -xvf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2`

- export PATH=$PATH:/path/to/compiler

    `export PATH=$PATH:~/gcc-arm-none-eabi-10.3-2021.10/bin`

- Another option is to add permanently to path. Add export cmd to either file that exists:

    ~/.bashrc
    ~/.bash_profile


### Modifications:

- ldscript:
  - pick target script (only diff is ROM and RAM origin and size)
- startup:
  - pick correct startup file: M0. M3, M4, M7
  - modify Stack_Size if needed
  - check interrupts at __isr_vector and weak handler definitions (def_irq_handler) - see issue with LTO!
- makefile:
  - update ldscript
  - update core
  - set optimizer level and LTO
  - update all files
- compile

### Flashing:

- Using STLink utility, set start to 0x0800000, flash binary (bin) and run

### Where files come from:

- From /share/gcc-arm-none-eabi/samples, extracted to project the following files:
  - ldscripts/*  - using nokeep.ld (minimum size binary)
  - startup/*
  - makefile

### Setting up the micro:

- Adjust HSE_VALUE definition in stm32f4xx.h and stm32f10x.h

# Plotting results in C simulation

- Install gnuplot (sudo apt install gnuplot)
- run command '/env/gcc/gnuplot plotcmd.txt'
- or run '/env/gcc/plot' script which will run make and gnuplot
- plot will be shown via 'display' command or open jpg manually