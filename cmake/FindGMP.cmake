# Include dir
STRING(REPLACE ":" ";" CPATH "$ENV{CPATH}")
find_path(GMP_INCLUDE_DIR
	NAMES gmp.h
	HINTS ${CPATH}
	DOC "Include directory for GMP"
)

# Library files
STRING(REPLACE ":" ";" LIBRARY_PATH "$ENV{LIBRARY_PATH}")
find_library(GMP_LIBRARY
	NAMES gmp
	HINTS ${LIBRARY_PATH}
	PATHS /usr/local/lib
)

# Version
function(GetVersionPart OUTPUT FILENAME DESC)
	file(STRINGS ${FILENAME} RES REGEX "^#define __GNU_MP_${DESC}[ \\t]+.*")
	string(REGEX MATCH "[0-9]+" RES "${RES}")
	set(${OUTPUT} "${RES}" PARENT_SCOPE)
endfunction()
function(GetVersion OUTPUT FILENAME)
	GetVersionPart(MAJOR "${FILENAME}" "VERSION")
	GetVersionPart(MINOR "${FILENAME}" "VERSION_MINOR")
	GetVersionPart(PATCH "${FILENAME}" "VERSION_PATCHLEVEL")
	set(${OUTPUT} "${MAJOR}.${MINOR}.${PATCH}" PARENT_SCOPE)
endfunction()

if(GMP_INCLUDE_DIR AND GMP_LIBRARY)
	set(GMP_FOUND TRUE)

	if (EXISTS "${GMP_INCLUDE_DIR}/gmp-x86_64.h")
		GetVersion(GMP_VERSION "${GMP_INCLUDE_DIR}/gmp-x86_64.h")
	elseif (EXISTS "${GMP_INCLUDE_DIR}/gmp-aarch64.h")
		GetVersion(GMP_VERSION "${GMP_INCLUDE_DIR}/gmp-aarch64.h")
	else()
		GetVersion(GMP_VERSION "${GMP_INCLUDE_DIR}/gmp.h")
	endif()

	if(GMP_FIND_VERSION VERSION_GREATER GMP_VERSION)
		message(WARNING "Required GMP ${GMP_FIND_VERSION} but found only GMP ${GMP_VERSION}.")
		unset(GMP_FOUND)
		unset(GMP_INCLUDE_DIR)
		unset(GMP_LIBRARY)
		unset(GMP_VERSION)
		return()
	endif()
endif()

mark_as_advanced(
	GMP_FOUND
	GMP_INCLUDE_DIR
	GMP_LIBRARY
	GMP_VERSION
)
