
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

set_version(14 32 1)

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
option( CARL_WARNING_AS_ERROR "Sets whether compile warnings are treated as errors." OFF)
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
option(ALLOW_SHIPPED_CLN "Should we build cln, if not available?" OFF)
option(ALLOW_SHIPPED_GINAC "Should we build ginac, if not available?" OFF)
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
if (PROJECT_IS_TOP_LEVEL)
	set(CARL_EXPORT_TO_CMAKE_DEFAULT ON)
else()
	set(CARL_EXPORT_TO_CMAKE_DEFAULT OFF)
	# Note that this is actually discouraged to do, but we keep this for some time.
endif()
option(CARL_EXPORT_TO_CMAKE "Export the project to CMake for easy inclusion" ${CARL_EXPORT_TO_CMAKE_DEFAULT})

if (CARL_EXPORT_TO_CMAKE)
	message(WARNING "carl - Exporting carl to the cmake registry is a discouraged practice, which we will discontinue as default in the future..")
endif()

option (CARL_COMPILE_BENCHMARKS "Compile Benchmarks" OFF)

set(CLANG_SANITIZER "none" CACHE STRING "Compile with the respective sanitizer")
set_property(CACHE CLANG_SANITIZER PROPERTY STRINGS none address memory thread)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (PROJECT_IS_TOP_LEVEL)
	if (NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "Build type." FORCE)
	endif()
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "DEBUG" "RELEASE")
endif()


set(CARL_LIBRARIES_DIR "${PROJECT_BINARY_DIR}/lib")

# Offer the user the choice of overriding the installation directories
set(CARL_INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for header files" )
set(CARL_LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
set(CARL_BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for executables")
set(CARL_CMAKE_INSTALL_DIR lib/cmake/ CACHE PATH "Installation directory for executables")

if (PROJECT_IS_TOP_LEVEL)
	set(CMAKE_INSTALL_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH  "Installation directory for CMake files")
endif()

if(APPLE AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES arm64)
    message(STATUS "carl - Detected that target system uses Apple Silicon.")
	set(APPLE_SILICON 1)
endif()

##
add_compile_options($<$<CONFIG:Release>:-O3>)
add_compile_options($<$<CONFIG:Debug>:-O1>)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # Matches "Clang" and "AppleClang"
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)
		message(FATAL_ERROR "carl - Clang version must be at least 3.5!")
	endif()
	if (APPLE)
		message(STATUS "carl - Using clang ${CMAKE_CXX_COMPILER_VERSION} on Apple")
		set(CMAKE_MACOSX_RPATH 1)
		add_compile_options(-stdlib=libc++)
	else()
		message(STATUS "carl - Using clang ${CMAKE_CXX_COMPILER_VERSION}")
		add_compile_options()
	endif()
	add_compile_options($<$<CONFIG:Release>:-fomit-frame-pointer>)
	add_compile_options($<$<CONFIG:Release>:-funroll-loops>)
	if (NOT "${CLANG_SANITIZER}" STREQUAL "none")
		add_compile_options(-fsanitize=${CLANG_SANITIZER})
	endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
		message(FATAL_ERROR "carl - g++ version must be at least 4.9!")
	endif()
	message(STATUS "carl - Using g++ ${CMAKE_CXX_COMPILER_VERSION}")
	add_compile_options(-Wunknown-pragmas)
	add_compile_options(-fmax-errors=5 -fdiagnostics-color=auto)
	if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13.0)
		add_compile_options(-Wno-stringop-overflow) # Lead to warnings in carl/formula/bitvector/BVValue.h
	endif()
else()
	message(WARNING "carl - Possibly unsupported compiler ${CMAKE_CXX_COMPILER_ID}")
endif()
set(CMAKE_COMPILE_WARNING_AS_ERROR ${CARL_WARNING_AS_ERROR})


# In release mode, we turn on even more optimizations if we do not have to provide a portable binary.
if (NOT PORTABLE AND (NOT APPLE_SILICON OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15.0)))
	add_compile_options($<$<CONFIG:Release>:-march=native>)
endif()

if(DEVELOPER)
	add_compile_options(-Wall -Wextra -Wconversion -Wno-deprecated-declarations)
	add_compile_options(-DDEBUG -ftemplate-backtrace-limit=0)

	if (ALLWARNINGS)
		if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
			add_compile_options(-Weverything)
			add_compile_options(-Wno-c++98-compat -Wno-c++98-compat-pedantic)
			add_compile_options(-Wno-old-style-cast)
			add_compile_options(-Wno-reserved-id-macro)
			add_compile_options(-Wno-newline-eof)
			add_compile_options(-Wno-documentation)
			add_compile_options(-Wno-weak-vtables)
			add_compile_options(-Wno-global-constructors)
			add_compile_options(-Wno-exit-time-destructors)
			add_compile_options(-Wno-switch-enum)
			add_compile_options(-Wno-covered-switch-default)
			add_compile_options(-Wno-padded)
			add_compile_options(-Wno-float-equal)
			add_compile_options(-Wno-unused-local-typedef)
			add_compile_options(-Wno-missing-variable-declarations)
			add_compile_options(-fsanitize=memory)
		endif()
		if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
			add_compile_options(-Weffc++)
		endif()
	endif()

endif()
message(STATUS "carl - Build type: ${CMAKE_BUILD_TYPE}")


foreach(p LIB BIN INCLUDE CMAKE)
  set(var CARL_${p}_INSTALL_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# path to put in the executables after building.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for built executables")


if(APPLE)
	set(DYNAMIC_EXT ".dylib")
	set(STATIC_EXT ".a")
else()
	set(DYNAMIC_EXT ".so")
	set(STATIC_EXT ".a")
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


# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")


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
	#include(coverage)
endif()

#dl must be linked LAST as this is required for the stacktrace in util/platform.h

list(APPEND carl_LIBRARIES_DYNAMIC dl)


if (TARGET Doxygen::doxygen AND PROJECT_IS_TOP_LEVEL)
	include(clang-tidy)
	include(resources/doc-resources.cmake)
	add_subdirectory(doxygen-conf)
endif()

if(PROJECT_IS_TOP_LEVEL)
	# Apply code formatting
	add_custom_target(format COMMAND ${PROJECT_SOURCE_DIR}/resources/scripts/auto-format.sh)
endif()

add_subdirectory(src)

include(export)
include(packaging)
