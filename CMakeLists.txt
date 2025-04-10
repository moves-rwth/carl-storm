
# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties.
# B. Options.
# C. Find system tools and libraries.
# D. Write information files.
# E. Invoke subdirectories.
# F. Export Compile Information
# G. CPack
# # # # # # # # # # # # # # # # # # # # # #

cmake_minimum_required (VERSION 3.22)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
	cmake_policy(SET CMP0135 NEW) # Use timestamps from time of extraction
endif()

set(CMAKE_CXX_STANDARD 20)

# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties
# # # # # # # # # # # # # # # # # # # # # #
project(carl CXX)

# path to find own modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# Include own macros.
include( carlmacros )

set_version(14 28 0)

set( PROJECT_FULLNAME "carl")
set( PROJECT_DESCRIPTION "Computer ARithmetic Library")
set( carl_NAME "CArL" )
set( carl_DESCRIPTION ${PROJECT_DESCRIPTION} )

message(STATUS "carl - Version: ${PROJECT_FULLNAME} ${PROJECT_VERSION_FULL}")

# # # # # # # # # # # # # # # # # # # # # #
# B. Options
# # # # # # # # # # # # # # # # # # # # # #

# options
option( DEVELOPER "Compile with extra warnings" OFF )
option( ALLWARNINGS "Compile with even more warnings" OFF )
option( CARL_LOGGING "Enable logging within the carl library" OFF )
export_option(CARL_LOGGING)
option( LOGGING_DISABLE_INEFFICIENT "Disable log messages about inefficient methods" OFF )
option( TIMING "Enable timing within the carl library" OFF )
export_option(TIMING)
option( FORCE_SHIPPED_RESOURCES "Do not look in system for resources which are included" OFF )
export_option(FORCE_SHIPPED_RESOURCES)
option( FORCE_SHIPPED_GMP "Do not look in system for lib gmp" OFF )
export_option(FORCE_SHIPPED_GMP)
option( USE_GINAC "Compile with testing with the runtime and result comparisons of carl to ginac" ON )
export_option(USE_GINAC)
option( USE_Z3_NUMBERS "Make z3 rationals available in carl" OFF )
option( USE_CLN_NUMBERS "Make cln rationals and integers available in carl" ON )
export_option(USE_CLN_NUMBERS)
option( USE_COCOA "Use CoCoALib" OFF )
export_option(USE_COCOA)
option( USE_BLISS "Use bliss" OFF )
export_option(USE_BLISS)
OPTION( USE_MPFR_FLOAT "Use the mpfr implementation of floating point numbers." OFF )
export_option(USE_MPFR_FLOAT)
option( PORTABLE "Enable portable build without hardware-specific compiler flags." OFF)
export_option(PORTABLE)
option( BUILD_STATIC "Build the static library as well" OFF )
export_option(BUILD_STATIC)
option( BUILD_DOXYGEN "Build doxygen if not found" OFF )
export_option(BUILD_DOXYGEN)
option( THREAD_SAFE "Use mutexing to assure thread safety" ON )
export_option(THREAD_SAFE)
option( PRUNE_MONOMIAL_POOL "Prune monomial pool" ON )
option( EXCLUDE_TESTS_FROM_ALL "If set, tests will not be compiled by default" OFF )
export_option(EXCLUDE_TESTS_FROM_ALL)

option (CARL_COMPILE_BENCHMARKS "Compile Benchmarks" OFF)

set(CLANG_SANITIZER "none" CACHE STRING "Compile with the respective sanitizer")
set_property(CACHE CLANG_SANITIZER PROPERTY STRINGS none address memory thread)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (PROJECT_IS_TOP_LEVEL)
	set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "Build type." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "DEBUG" "RELEASE")
endif()


set(CARL_LIBRARIES_DIR "${PROJECT_BINARY_DIR}/lib")

# Offer the user the choice of overriding the installation directories
set(CARL_INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for header files" )
set(CARL_LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
set(CARL_BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for executables")
set(CARL_CMAKE_INSTALL_DIR lib/cmake/ CACHE PATH "Installation directory for executables")



if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/carl)
endif()
if (PROJECT_IS_TOP_LEVEL)
	set(CMAKE_INSTALL_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH  "Installation directory for CMake files")
endif()

if(APPLE AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES arm64)
    message(STATUS "carl - Detected that target system uses Apple Silicon.")
	message(WARNING "carl - Compiling natively on Apple Silicon is experimental. Please report issues to support@stormchecker.org. For more information visit https://www.stormchecker.org/documentation/obtain-storm/apple-silicon.html")
	set(APPLE_SILICON 1)
endif()

foreach(p LIB BIN INCLUDE CMAKE)
  set(var CARL_${p}_INSTALL_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# path to put in the executables after building.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for built executables")

include(compiler-options)

set(DYNAMIC_EXT ".so")
set(STATIC_EXT ".a")
if(APPLE)
	set(DYNAMIC_EXT ".dylib")
	set(STATIC_EXT ".a")
elseif (WIN32)
	set(DYNAMIC_EXT ".dll")
	set(STATIC_EXT ".lib")
endif()

if(BUILD_STATIC)
	message(STATUS "carl - Building static: yes")
	if (LOGGING)
	message(WARNING "carl - A static build with logging enabled will probably trigger a segfault!")
	endif()
else()
	message(STATUS "carl - Building static: no")
endif()

# RPATH settings
# don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already (but only when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# # # # # # # # # # # # # # # # # # # # # #
# C. Find system tools and libraries.
#
# 1. Required libraries for core.
# 2. Optional libraries for core.
# 3. Development and debug libraries.
# # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # # # #
# C.1. Required libraries for core.
# # # # # # # # # # # # # # # # # # # # # #

include(resources/resources.cmake)

# we only search for this libraries in the system, if we do not force using the shipped resources.
if(PROJECT_IS_TOP_LEVEL)
	enable_testing()
	include(coverage)
endif()

#dl must be linked LAST as this is required for the stacktrace in util/platform.h
if (NOT WIN32)
	list(APPEND carl_LIBRARIES_DYNAMIC pthread dl)
endif()

if (TARGET Doxygen::doxygen AND PROJECT_IS_TOP_LEVEL)
	include(clang-tidy)
	include(resources/doc-resources.cmake)
	add_subdirectory(doxygen-conf)
endif()
add_subdirectory(src)

include(export)

include(resources/addons/addons.cmake)
include(packaging)
