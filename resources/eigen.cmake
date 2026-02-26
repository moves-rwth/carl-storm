ExternalProject_Add(
		eigen_carl_src
		GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
		# This tag should be set from Storm to coincide with Storm's Eigen version
		GIT_TAG ${CARL_EIGEN_GIT_TAG}
		SOURCE_DIR ${PROJECT_BINARY_DIR}/resources/Eigen
		# First check whether patch was already applied (--reverse --check), otherwise apply patch
		#PATCH_COMMAND git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen501.patch --reverse --check -q || git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen501.patch
		UPDATE_COMMAND ""
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
		LOG_INSTALL ON
)
add_library(eigencarl INTERFACE) # Not imported, we are in control of the sources.
add_dependencies(eigencarl eigen_carl_src)
# note that we include without Eigen
target_include_directories(eigencarl INTERFACE
		$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/resources/Eigen>
		$<INSTALL_INTERFACE:${CARL_INCLUDE_INSTALL_DIR}/carl/resources/Eigen>
)
install(TARGETS eigencarl EXPORT carl_Targets)
install(DIRECTORY ${PROJECT_BINARY_DIR}/resources/Eigen/Eigen
		DESTINATION ${CARL_INCLUDE_INSTALL_DIR}/carl/resources/Eigen
)
list(APPEND CARL_TARGETS eigencarl)
