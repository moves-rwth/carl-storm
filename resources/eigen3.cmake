ExternalProject_Add(
		eigen_carl_src
		GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
		GIT_TAG bae907b8f6078b1df290729eef946360315bd312
		SOURCE_DIR ${PROJECT_BINARY_DIR}/resources/Eigen
		# First check whether patch was already applied (--reverse --check), otherwise apply patch
		PATCH_COMMAND "" #$git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen341alpha.patch --reverse --check || git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen341alpha.patch
		UPDATE_COMMAND ""
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
		LOG_INSTALL ON
)
add_library(eigen3carl INTERFACE) # Not imported, we are in control of the sources.
add_dependencies(eigen3carl eigen_carl_src)
# note that we include without Eigen
target_include_directories(eigen3carl INTERFACE
		$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/resources/Eigen>
		$<INSTALL_INTERFACE:${CARL_INCLUDE_INSTALL_DIR}/carl/resources/Eigen>
)
install(TARGETS eigen3carl EXPORT carl_Targets)
install(DIRECTORY ${PROJECT_BINARY_DIR}/resources/Eigen/Eigen
		DESTINATION ${CARL_INCLUDE_INSTALL_DIR}/carl/resources/Eigen
)
list(APPEND CARL_TARGETS eigen3carl)