include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.zip
)
FetchContent_MakeAvailable(googletest)

set(GTEST_LIBRARIES GTest::gtest_main)
