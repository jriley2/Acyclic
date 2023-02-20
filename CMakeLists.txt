cmake_minimum_required(VERSION 3.10)

# set the project name
project(Acyclic VERSION 0.1.0 DESCRIPTION "Acyclic Task Scheduler")

option(USE_PROFILER "Turn on trace execution" OFF)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set a default build type if none was specified
set(default_build_type "Release")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#add compiler flags
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -pedantic -Wno-unused-parameter")
endif()

message(STATUS "Building in ${CMAKE_BUILD_TYPE} mode")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "C++ compile flags: ${CMAKE_CXX_FLAGS}")
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "C++ release compile flags: ${CMAKE_CXX_FLAGS_RELEASE}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "C++ debug compile flags: ${CMAKE_CXX_FLAGS_DEBUG}")
endif ()

enable_testing()

add_subdirectory(lib)
add_subdirectory(apps)

install(FILES LICENSE.txt README.md DESTINATION "doc/")
install(FILES LICENSE.txt README.md DESTINATION ".")

### DOXYGEN

find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/acyclic.dox.conf.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/acyclic.dox.conf)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    message(STATUS "Build docs by running 'make docs'")

    add_custom_target(
    doxygen
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
    )
else ()
    message(STATUS "Doxygen not found. Can not build docs.")
endif (DOXYGEN_FOUND)

### CPACK

SET(CPACK_GENERATOR "TGZ")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
INCLUDE(CPack)
message(STATUS "cpack is using generator ${CPACK_GENERATOR}")
