# Acyclic

## Description

##  Getting started with the Acyclic

## Building

### Prerequisites

* [Git](https://git-scm.com/)
* [CMake](https://cmake.org/) -- minimum version 3.7
* c++ compiler with c++14

On Debian:

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

## Build your own application

1. Build Acyclic
2. Include headers when compiling your application
3. Link library

## Disclaimer/Copyright

See "LICENSE.txt" in this directory.
