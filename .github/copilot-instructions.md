# Project Overview

This is a embedded oriented project, that shall be tailored to Raspberry PI's pico. It shall create a basic shell built on top of FreeRtos, that will, in the end, have
commands such as: ls, cd, cat, etc. The output will be displaied on the serial monitor, at fist, but i intend on adding a small display.

## Folder Structure

-> % tree -L 1 ..
..
├── build
├── cmake
├── CMakeLists.txt
├── config
├── documentation
├── inc
├── lib
├── libs
├── LICENSE
├── Makefile
├── README.md
├── src
└── utils

## Libraries and Frameworks

- FreeRTOS for the os
- FatFs for the filesystem
- pico-sdk for the drivers
- ulibSD for the SD driver. Reading sectors from the SD

## Coding Standards

- Follow misra rules

## Where do libraries go

Generally you clone the repo into the libs if its not a static library, if static you put it into lib, otherwise into libs.
Then link it with cmake by creating a <library>.cmake file and hook that to CMakeLists. If the library
needs config files, that goes into the config/ folder.

