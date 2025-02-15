ExternalProject_Add(
    Eigen3-EP
	GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
	GIT_TAG ${EIGEN3_VERSION}
	DOWNLOAD_NO_PROGRESS 1
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DPKGCONFIG_INSTALL_DIR=<INSTALL_DIR>/lib/pkgconfig
	LOG_INSTALL 1
)

ExternalProject_Get_Property(Eigen3-EP INSTALL_DIR)

add_imported_library(EIGEN3 SHARED "" "${INSTALL_DIR}/include/eigen3")

add_dependencies(carl_resources Eigen3-EP)
