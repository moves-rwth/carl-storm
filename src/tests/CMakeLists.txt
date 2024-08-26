
add_custom_target(all-tests)

add_library(TestCommon STATIC Common.cpp)
target_link_libraries(TestCommon lib_carl ${GTEST_LIBRARIES})
add_dependencies(TestCommon carl_resources lib_carl GTESTCORE_STATIC GTESTMAIN_STATIC)

################################
# Unit Tests
################################
add_subdirectory(util)
add_subdirectory(numbers)
#add_subdirectory(numbers_boost)
add_subdirectory(core)
add_subdirectory(formula)
add_subdirectory(interval)
# Only for debugging.
#add_subdirectory(debug)

if (CLANG_TIDY)
	add_custom_target(tidy-tests
		COMMAND ${CLANG_TIDY} -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${carl_lib_sources}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/carl/
	)
	file(GLOB dirs LIST_DIRECTORIES true RELATIVE "${PROJECT_SOURCE_DIR}/src/tests/" "*")
	foreach(dir ${dirs})
		if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/src/tests/${dir}")
			file(GLOB_RECURSE sources_${dir} "${PROJECT_SOURCE_DIR}/src/tests/${dir}/*.cpp")
			add_custom_target(tidy-tests-${dir}
				COMMAND ${CLANG_TIDY} -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${sources_${dir}}
				WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/tests/
			)
		endif()
	endforeach()
endif()
