include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${PROJECT_BINARY_DIR}/resources)


add_custom_target(carl_resources)
if(PROJECT_TOP_LEVEL)
add_custom_target(resources)
add_dependencies(resources carl_resources)
endif()

###############
##### Generic resource configuration
###############
if("${CMAKE_GENERATOR}" MATCHES "Make")
	set(CMAKE_MAKE_PROGRAM "$(MAKE)")
endif()

# Make sure that libraries from /usr/lib et al are found before OSX frameworks
set(CMAKE_FIND_FRAMEWORK "LAST")

function(print_resource_info name target version)
	if(TARGET ${target})
		get_target_property(TYPE ${target} TYPE)
		if(TYPE STREQUAL "EXECUTABLE")
			get_target_property(PATH1 ${target} IMPORTED_LOCATION)
		elseif(TYPE STREQUAL "SHARED_LIBRARY")
			get_target_property(PATH1 ${target} INTERFACE_INCLUDE_DIRECTORIES)
			get_target_property(PATH2 ${target} IMPORTED_LOCATION)
		elseif(TYPE STREQUAL "STATIC_LIBRARY")
			get_target_property(PATH1 ${target} INTERFACE_INCLUDE_DIRECTORIES)
			get_target_property(PATH2 ${target} IMPORTED_LOCATION)
		elseif(TYPE STREQUAL "INTERFACE_LIBRARY")
			get_target_property(PATH1 ${target} INTERFACE_INCLUDE_DIRECTORIES)
			get_target_property(PATH2 ${target} INTERFACE_LINK_LIBRARIES)
		endif()
		if(PATH1 AND PATH2)
			message(STATUS "carl - ${name} ${version} was found at ${PATH1} and ${PATH2}")
		else()
			message(STATUS "carl - ${name} ${version} was found at ${PATH1}")
		endif()
	else()
		message(STATUS "carl - ${name} was not found.")
	endif()
endfunction(print_resource_info)

###############
##### Process resource dependencies
###############
if(USE_GINAC)
	set(USE_CLN_NUMBERS ON)
endif()

###############
##### Load resources
#####
##### Note that the resources may have dependencies among each other.
###############

##### GMP / GMPXX
if((NOT FORCE_SHIPPED_RESOURCES) AND (NOT FORCE_SHIPPED_GMP))
	load_library(carl GMP 6.1)
	load_library(carl GMPXX 6.1)
endif()
if(NOT GMP_FOUND)
	set(GMP_VERSION "6.1.2")
	include(resources/gmp.cmake)
endif()
print_resource_info("GMP / GMPXX" GMP_SHARED ${GMP_VERSION})

##### Boost
set(BOOST_COMPONENTS "regex")
set(Boost_USE_DEBUG_RUNTIME OFF)
set(Boost_NO_BOOST_CMAKE ON)
load_library(carl Boost 1.58 COMPONENTS ${BOOST_COMPONENTS})
if(NOT Boost_FOUND)
	message(STATUS "carl - BOOST required but not found.")
endif()
print_resource_info("Boost" Boost_SHARED ${Boost_VERSION})

##### Eigen3
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl EIGEN3 3.3)
endif()
if(NOT EIGEN3_FOUND)
	set(EIGEN3_VERSION "3.4.0")
	include(resources/eigen3.cmake)
endif()
print_resource_info("Eigen3" EIGEN3 ${EIGEN3_VERSION})

##### bliss
if(USE_BLISS)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl BLISS 0.73)
	endif()
	if(NOT BLISS_FOUND)
		set(BLISS_VERSION "0.73")
		include(resources/bliss.cmake)
	endif()
	print_resource_info("Bliss" BLISS_SHARED ${BLISS_VERSION})
else()
	message(STATUS "carl - Bliss is disabled")
endif()

##### CLN
if(USE_CLN_NUMBERS)
	if(NOT FORCE_SHIPPED_RESOURCES)
		if(APPLE_SILICON)
			load_library(carl CLN 1.3.7)
		else()
			load_library(carl CLN 1.3)
		endif()
	endif()
	if(NOT CLN_FOUND)
		set(CLN_VERSION "1.3.7")
		include(resources/cln.cmake)
	endif()
	set_target_properties(CLN_STATIC PROPERTIES LINK_INTERFACE_LIBRARIES "GMP_STATIC")
	print_resource_info("CLN" CLN_SHARED ${CLN_VERSION})
else()
	message(STATUS "carl - CLN is disabled")
endif()


##### CoCoALib
if(USE_COCOA)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl CoCoA 0.99564)
	endif()
	if(NOT COCOA_FOUND)
		set(COCOA_VERSION "0.99564")
		set(COCOA_TGZHASH "1cfc3b6ff549ea01e1bffaf3bc9600a8")
		include(resources/cocoa.cmake)
		unset(COCOA_TGZHASH)
	endif()
	print_resource_info("CoCoA" COCOA_SHARED ${COCOA_VERSION})
else()
	message(STATUS "carl - CoCoA is disabled")
endif()

##### GiNaC
if(USE_GINAC)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl GINAC 1.7)
	endif()
	if(NOT GINAC_FOUND)
		set(GINAC_VERSION "1.8.7")
		include(resources/ginac.cmake)
	endif()
	print_resource_info("GiNaC" GINAC_SHARED ${GINAC_VERSION})
else()
	message(STATUS "carl - GiNaC is disabled")
endif()


##### GTest
if(NOT GTEST_FOUND)
	set(GTEST_VERSION "1.8.0")
	set(GTEST_ZIPHASH "adfafc8512ab65fd3cf7955ef0100ff5")
	include(resources/gtest.cmake)
	unset(GTEST_ZIPHASH)
endif()
print_resource_info("GTest" GTESTMAIN_STATIC ${GTEST_VERSION})

##### MPFR
IF(USE_MPFR_FLOAT)
	load_library(carl MPFR 0.0 REQUIRED)
endif()
