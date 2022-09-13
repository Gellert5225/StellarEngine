# Stellar Engine - Vulkan
A custom game engine using C++ and Vulkan API
## Table of Contents
* [Introduction](#introduction)
* [Installation](#installation)
* [ToDo](#todo)

## Introduction
Stellar Engine is a cross-platform game engine using Vulkan API. Inspired by Cherno's [Hazel Engine](https://github.com/TheCherno/Hazel)

Currently it is at the very initial state of development. It supports Windows10(64 bit), Linux(64 bit) and macOS with Intel CPU. 
> Although theoretically it should support Apple Silicon but I do not have a M1 Mac

### Current Stage
![](Screenshots/currentStage.png)

## Installation

### Prerequisite

I will try to list as detailed as possible. However I have not tested the build on a fresh envrionment so you are welcomed to try it and provide some feedback.

- [Vulkan](https://vulkan.lunarg.com/)
    - [Windows](https://vulkan.lunarg.com/doc/sdk/latest/windows/getting_started.html)
    - [macOS](https://vulkan.lunarg.com/doc/sdk/latest/mac/getting_started.html)
    - [Linux](https://vulkan.lunarg.com/doc/view/latest/linux/getting_started_ubuntu.html)
- Follow the Development Environment Tutorial (https://vulkan-tutorial.com/Development_environment)
- C++17
- Windows:
    - MinGW with GCC installed
    - Follow tutorial here: https://www.msys2.org/
- Linux:
    - GNU GCC
    - X11
- macOS:
    - Clang++

### Running

To run, you need to compile the core engine first:

`cd Stellar && make`

Then, depending on your platform, a shared library will be generated and copied to `Sandbox/bin`. (`.dll` for Win64 and `.so` for Linux) 

Now you just need to build the Sandbox project:

`cd .. && cd Sandbox && make && make run`

## ToDo

- [x] Window abstraction
- [x] Layer
- [x] Events
- [x] ImGui
- [x] Triangle
- [x] Shader library
- [x] Camera
- [ ] Texture
- [x] Renderer
- [ ] Metal-cpp
- [ ] macOS support
- [ ] Load model
- [ ] PBR
- [ ] IBL
- [ ] Deferred rendering
- [ ] Networking