### Files ###

Here is an overview of the files found in this project:

* .gdbinit: contains startup commands for gdb, the debugger.
  If you don't want to use this file, you will have to enter the last two lines
  every time when you start gdb: `target extended localhost:4242` and `load`.
  This file is a copy of the
  [original by Ross Wolin](https://github.com/rowol/stm32_discovery_arm_gcc/blob/master/blinky/.gdbinit).
* Makefile: commented makefile used to compile the GraphicLCDController project.
  You can use this file to see how the whole compilation process works and how
  to use the tools.
* main.c: this is the actual program (with comments).
* stm32\_flash.ld: linker script provided by ST. Contains a description of the
  memory layout used by the Cortex-M4.
* stm32f4xx\_conf.h: this file is used to include the header files of the
  standard peripheral library. It is a configuration file.
  You could include those headers directly in
  main.c, but the peripheral library includes this configuration header
  (stm32f4xx.h includes it conditionally),
  so we must provide it - or else we cannot compile our code against the library.
  This file is provided by ST and is supposed to be adapted to every project.
* system\_stm32f4xx.c: implements intialization code called from the startup file
  startup\_stm32f4xx.s (present in the library).
  This file is provided by ST.

#### Missing files ####

This project does not include the peripheral library used to build the
program. You can however download the library from ST (see link below).
In my setup I have extracted the library in a folder one level above the
"GraphicLCDController" folder. That is I have the following directory structure:


    - stm +
          |- STM32F4-Discovery_FW_V1.1.0/ (In .gitignore)
          |          +
          |          | - Libraries
          |          | - MCD-ST Liberty SW License Agreement 20Jul2011 v0.1.pdf
          |          | - Project
          |          | - Release_Notes.html
          |          | - Utilities
          |          | - _htmresc
          |
          |- GraphicLCDController/ +
                     | - .gdbinit
                     | - Makefile
                     | - Readme.md
                     | - main.c
                     | - stm32_flash.ld
                     | - stm32f4xx_conf.h
                     | - system_stm32f4xx.c

                     
If you search STM32F4-Discovery_FW_V1.1.0 / STM32F4-Discovery_FW_V1.1.0.tar.gz on the web you will find that the library is offered in many projects.

### Tools and software requirements ###

* Boot device in DFU mode:
Can be activated with pins BOOT0=1 and BOOT1=0 at startup. 

Normally you only need to short BOOT1 to ground while pressing reset, after that the device is recognised as a DFU device which can be programmed with DFU-Util.

After that put BOOT0 to ground, when powering up the second led on the STM board should burn.

* Installation of required tools on Ubuntu 16.0.4
```
sudo apt install dfu-util gcc-arm-linux-gnueabi binutils-arm-linux-gnueabi gcc-arm-none-eabi gdb-arm-none-eabi binutils-arm-none-eabi
```
* dfu-util commands:
```
dfu-util -a 0 -s 0x08000000:leave -D graphiclcdcontroller.bin
```
* To compile and send the binary to the stm board do:
```
cd stm/GraphicLCDController
make
dfu-util -a 0 -s 0x08000000:leave -D graphiclcdcontroller.bin
```


### Thanks ###

* Many thanks to [Malkavian](https://github.com/Malkavian). This project has been based on Malkavian 's stm32 blink tutorial at [blinky](https://github.com/Malkavian/tuts/tree/master/stm/blinky)

* Many thanks to [Ross Wolin](https://github.com/rowol) for writting
  [this article](http://www.wolinlabs.com/blog/linux.stm32.discovery.gcc.html)
  where he shows how to use a makefile to build an application on Linux against
  the standard peripheral library for the STM32F4 board.

  The makefile found in this project is based on Ross Wolin's
  [makefile](https://github.com/rowol/stm32_discovery_arm_gcc/blob/master/blinky/Makefile).
