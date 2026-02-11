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
print_resource_info("GMP / GMPXX" GMP_SHARED ${GMP_VERSION})

##### Boost

find_package(Boost 1.70 REQUIRED CONFIG)
print_resource_info("Boost" Boost::headers ${Boost_VERSION})

##### Eigen3
set(EIGEN3_VERSION "3.4.1-alpha")
include(resources/eigen3.cmake)
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
set(HAVE_CLN FALSE)
export_option(HAVE_CLN)
if(USE_CLN_NUMBERS)
	if(NOT FORCE_SHIPPED_RESOURCES)
		if(APPLE_SILICON)
			load_library(carl CLN 1.3.7)
		else()
			load_library(carl CLN 1.3)
		endif()
	endif()
	if(NOT CLN_FOUND AND ALLOW_SHIPPED_CLN)
		set(CLN_VERSION "1.3.7")
		include(resources/cln.cmake)
		set(HAVE_CLN TRUE)
	endif()
	if(CLN_FOUND OR ALLOW_SHIPPED_CLN)
		set_target_properties(CLN_STATIC PROPERTIES LINK_INTERFACE_LIBRARIES "GMP_STATIC")
		print_resource_info("CLN" CLN_SHARED ${CLN_VERSION})
		set(HAVE_CLN TRUE)
	else()
		message(STATUS "carl - CLN is disabled")
	endif()
else()
	message(STATUS "carl - CLN is disabled")
endif()
set(USE_CLN_NUMBERS ${HAVE_CLN})


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
set(HAVE_GINAC FALSE)
export_option(HAVE_GINAC)
if(USE_GINAC AND HAVE_CLN)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl GINAC 1.8)
	endif()
	if(NOT GINAC_FOUND AND ALLOW_SHIPPED_GINAC)
		set(GINAC_VERSION "1.8.10")
		include(resources/ginac.cmake)
	        set(HAVE_GINAC TRUE)
	endif()
	if(GINAC_FOUND OR ALLOW_SHIPPED_GINAC)
		print_resource_info("GiNaC" GINAC_SHARED ${GINAC_VERSION})
		set(HAVE_GINAC TRUE)
	else()
		message(STATUS "carl - GiNaC is disabled")
	endif()
else()
	message(STATUS "carl - GiNaC is disabled")
endif()
set(USE_GINAC ${HAVE_GINAC})

##### Threads
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

##### Googletest
if(PROJECT_IS_TOP_LEVEL)
	##### GTest
	if(NOT GTEST_FOUND)
		# Manual commit hash used in gtest.cmake to fix compiler warning
		#set(GTEST_VERSION "1.17.0")
		set(GTEST_VERSION "fa8438ae6b70c57010177de47a9f13d7041a6328")
		include(resources/gtest.cmake)
	endif()
	print_resource_info("GTest" GTest::gtest_main ${GTEST_VERSION})
endif()

##### MPFR
IF(USE_MPFR_FLOAT)
	load_library(carl MPFR 0.0 REQUIRED)
endif()

