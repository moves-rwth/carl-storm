find_program(PYTHON3 python3)
if(NOT PYTHON3)
	message(SEND_ERROR "Can not build GiNaC, missing binary for Python")
endif()

string(REPLACE "." "-" GINAC_TAG ${GINAC_VERSION})

ExternalProject_Add(
	GiNaC-EP
	# Use archive instead of Git repository because availability of Git repo was not stable enough
	URL https://www.ginac.de/ginac-${GINAC_VERSION}.tar.bz2
	#GIT_REPOSITORY "git://www.ginac.de/ginac.git"
	#GIT_TAG "release_${GINAC_TAG}"
	DOWNLOAD_NO_PROGRESS 1
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_INSTALL_LIBDIR=lib
	BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE} --target ginac

	BUILD_BYPRODUCTS ${BINARY_DIR}/lib/libginac${DYNAMIC_EXT} ${BINARY_DIR}/lib/libginac${STATIC_EXT}
	#	STEP_TARGETS install build
)


ExternalProject_Get_Property(GiNaC-EP INSTALL_DIR)

add_imported_library(GINAC SHARED "${INSTALL_DIR}/lib/libginac${DYNAMIC_EXT}" "${INSTALL_DIR}/include/ginac/")
#add_imported_library(GINAC STATIC "${INSTALL_DIR}/lib/libginac${STATIC_EXT}" "${INSTALL_DIR}/include/ginac/")

add_dependencies(GiNaC-EP CLN_SHARED CLN_STATIC)
add_dependencies(GINAC_SHARED GiNaC-EP)
#add_dependencies(GINAC_STATIC GiNaC-EP)
add_dependencies(carl_resources GINAC_SHARED GINAC_STATIC)
#add_dependencies(carl_resources CLN_STATIC GINAC_STATIC)
install(IMPORTED_RUNTIME_ARTIFACTS GINAC_SHARED DESTINATION ${CARL_LIB_INSTALL_DIR})

