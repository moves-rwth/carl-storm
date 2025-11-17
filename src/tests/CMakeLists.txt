
add_custom_target(all-tests)

add_library(TestCommon STATIC Common.cpp)
# Workaround for googletest bug with char conversions, being recognized by Clang 21+
# See https://github.com/google/googletest/issues/4762
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "21")
	message(WARNING "Googletest has a bug recognized by Clang 21+. Supressing character-conversion warning.")
	target_compile_options(TestCommon INTERFACE -Wno-character-conversion)
endif()

target_link_libraries(TestCommon lib_carl ${GTEST_LIBRARIES})
add_dependencies(TestCommon carl_resources lib_carl GTest::gtest_main)

################################
# Unit Tests
################################
add_subdirectory(util)
add_subdirectory(numbers)
add_subdirectory(core)
add_subdirectory(formula)
add_subdirectory(interval)

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
