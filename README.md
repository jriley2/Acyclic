# Acyclic

## Description

An acyclic task scheduler written in C++17.

##  Getting started with the Acyclic

A sample app is in apps/demo.

## Building

### Prerequisites

* [Git](https://git-scm.com/)
* [CMake](https://cmake.org/) -- minimum version 3.7
* c++ compiler with c++17

On Debian/Ubuntu:

      apt install git cmake build-essential

#### Optional

* cmake-gui
* [Doxygen](http://www.doxygen.nl/) -- For building documentation

### Build

      git clone https://jrriley@bitbucket.org/jrriley/acyclic.git
      cd acyclic
      mkdir build && cd build
      cmake ../ <configuration flags>
      make

CMAKE_BUILD_TYPE=value  

### Test

      cd <build dir>
      ctest

### Doxygen
To generate documentation:  

      cd <build dir>  
      make doxygen  

### Package
      cd <build dir>
      cpack

## Disclaimer/Copyright

See "LICENSE.txt" in this directory.
