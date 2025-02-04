include(CMakeDependentOption)

option(BUILD_ADDONS "Build addons" OFF)
cmake_dependent_option(BUILD_ADDON_PARSER "Build parser addon" OFF "BUILD_ADDONS" OFF)

if(BUILD_ADDONS)

	add_custom_target(addons)
	add_dependencies(addons lib_carl)
	set_directory_properties(PROPERTIES EP_PREFIX ${PROJECT_BINARY_DIR}/addons)

	if(BUILD_ADDON_PARSER)
		if(NOT USE_GINAC)
			message(WARNING "carl - Parser addon requires ginac!")
		endif()
		include(resources/addons/carl-parser.cmake)
	endif()
endif()
