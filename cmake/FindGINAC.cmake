# Include dir
find_path(GINAC_INCLUDE_DIR
	NAMES ginac.h
	HINTS ${GINAC_PKGCONF_INCLUDE_DIRS}
	PATHS
		/usr/include
		/usr/local/include
	PATH_SUFFIXES ginac
	DOC "Include directory for GiNaC"
)

find_library(GINAC_LIBRARY
	NAMES ginac
	HINTS ${GINAC_PKGCONF_LIBRARY_DIRS}
	PATHS
		/usr/lib
		/usr/local/lib
)

if(GINAC_INCLUDE_DIR AND GINAC_LIBRARY)
	set(GINAC_FOUND TRUE)

	# Version
	function(GetVersionPart OUTPUT FILENAME DESC)
		file(STRINGS ${FILENAME} RES REGEX "^#define GINACLIB_${DESC}_VERSION[ \\t]+.*")
		string(REGEX MATCH "[0-9]+" RES "${RES}")
		set(${OUTPUT} "${RES}" PARENT_SCOPE)
	endfunction()
	GetVersionPart(MAJOR "${GINAC_INCLUDE_DIR}/version.h" "MAJOR")
	GetVersionPart(MINOR "${GINAC_INCLUDE_DIR}/version.h" "MINOR")
	GetVersionPart(MICRO "${GINAC_INCLUDE_DIR}/version.h" "MICRO")
	set(GINAC_VERSION "${MAJOR}.${MINOR}.${MICRO}")

	if(GINAC_FIND_VERSION VERSION_GREATER GINAC_VERSION)
		message(WARNING "Required GiNaC ${GINAC_FIND_VERSION} but found only GiNaC ${GINAC_VERSION}.")
		return()
	endif()
endif()

mark_as_advanced(
	GINAC_FOUND
	GINAC_INCLUDE_DIR
	GINAC_LIBRARY
	GINAC_VERSION
)
