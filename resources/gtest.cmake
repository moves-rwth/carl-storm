include(FetchContent)
FetchContent_Declare(
	googletest
	# Use commit with workaround for char conversion warning, being recognized by Clang 21+.
	# See https://github.com/google/googletest/issues/4762
	URL https://github.com/google/googletest/archive/${GTEST_VERSION}.zip
	#URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.zip
)
FetchContent_MakeAvailable(googletest)

set(GTEST_LIBRARIES GTest::gtest_main Threads::Threads)
