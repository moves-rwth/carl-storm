string(REPLACE "." "-" CLN_TAG ${CLN_VERSION})

ExternalProject_Add(
	CLN-EP
	# Use archive instead of Git repository because availability of Git repo was not stable enough
	URL https://www.ginac.de/CLN/cln-${CLN_VERSION}.tar.bz2
	#GIT_REPOSITORY "git://www.ginac.de/cln.git"
	#GIT_TAG "cln_${CLN_TAG}"
	DOWNLOAD_NO_PROGRESS 1
	# Archive only comes with autotools instead of CMake
	CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-static
	BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
	#CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	#BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE} --target cln
	LOG_INSTALL 1
	BUILD_BYPRODUCTS ${INSTALL_DIR}/lib/libcln${DYNAMIC_EXT} ${INSTALL_DIR}/lib/libcln${STATIC_EXT}
)

ExternalProject_Get_Property(CLN-EP INSTALL_DIR)

add_imported_library(CLN SHARED "${INSTALL_DIR}/lib/libcln${DYNAMIC_EXT}" "${INSTALL_DIR}/include")
add_imported_library(CLN STATIC "${INSTALL_DIR}/lib/libcln${STATIC_EXT}" "${INSTALL_DIR}/include")

add_dependencies(CLN_SHARED CLN-EP)
add_dependencies(CLN_STATIC CLN-EP)
add_dependencies(carl_resources CLN_SHARED CLN_STATIC)
