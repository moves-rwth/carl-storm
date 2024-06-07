find_program(AUTORECONF autoreconf)
if(NOT AUTORECONF)
	message(SEND_ERROR "Can not build GiNaC, missing binary for autoreconf")
endif()

find_program(PYTHON3 python3)
if(NOT PYTHON3)
	message(SEND_ERROR "Can not build GiNaC, missing binary for Python")
endif()

string(REPLACE "." "-" GINAC_TAG ${GINAC_VERSION})

ExternalProject_Add(
	GiNaC-EP
	#URL https://www.ginac.de/ginac-${GINAC_VERSION}.tar.bz2
	GIT_REPOSITORY "git://www.ginac.de/ginac.git"
	GIT_TAG "release_${GINAC_TAG}"
	DOWNLOAD_NO_PROGRESS 1
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND ${AUTORECONF} -iv <SOURCE_DIR>
	COMMAND <SOURCE_DIR>/configure --quiet --prefix=<INSTALL_DIR> PYTHON=${PYTHON3} PKG_CONFIG_PATH=<INSTALL_DIR>/lib/pkgconfig/
	BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -C ginac
	INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} -C ginac install
	LOG_INSTALL 1
)

ExternalProject_Get_Property(GiNaC-EP INSTALL_DIR)

add_imported_library(GINAC SHARED "${INSTALL_DIR}/lib/libginac${DYNAMIC_EXT}" "${INSTALL_DIR}/include")
add_imported_library(GINAC STATIC "${INSTALL_DIR}/lib/libginac${STATIC_EXT}" "${INSTALL_DIR}/include")

add_dependencies(GiNaC-EP CLN_SHARED CLN_STATIC)
add_dependencies(GINAC_SHARED GiNaC-EP)
add_dependencies(GINAC_STATIC GiNaC-EP)
add_dependencies(resources GINAC_SHARED GINAC_STATIC)

mark_as_advanced(AUTORECONF)
