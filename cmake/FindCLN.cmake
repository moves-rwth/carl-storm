# Include dir
find_path(CLN_INCLUDE_DIR
	NAMES cln/cln.h
	HINTS ${CLN_PKGCONF_INCLUDE_DIRS}
	PATHS
		/usr/include
		/usr/local/include
)

# Library files
find_library(CLN_LIBRARY
	NAMES cln
	HINTS ${CLN_PKGCONF_LIBRARY_DIRS}
	PATHS
		/usr/lib
		/usr/local/lib
)

if(CLN_INCLUDE_DIR AND CLN_LIBRARY)
    set(CLN_FOUND TRUE)

	# Parse version
	file(STRINGS ${CLN_INCLUDE_DIR}/cln/version.h CLN_VERSION_MAJOR REGEX "^#define[\t ]+CL_VERSION_MAJOR .*")
	file(STRINGS ${CLN_INCLUDE_DIR}/cln/version.h CLN_VERSION_MINOR REGEX "^#define[\t ]+CL_VERSION_MINOR .*")
	file(STRINGS ${CLN_INCLUDE_DIR}/cln/version.h CLN_VERSION_PATCH REGEX "^#define[\t ]+CL_VERSION_PATCHLEVEL .*")
	string(REGEX MATCH "[0-9]+" CLN_VERSION_MAJOR "${CLN_VERSION_MAJOR}")
	string(REGEX MATCH "[0-9]+" CLN_VERSION_MINOR "${CLN_VERSION_MINOR}")
	string(REGEX MATCH "[0-9]+" CLN_VERSION_PATCH "${CLN_VERSION_PATCH}")
	set(CLN_VERSION "${CLN_VERSION_MAJOR}.${CLN_VERSION_MINOR}.${CLN_VERSION_PATCH}")

	if(CLN_FIND_VERSION VERSION_GREATER CLN_VERSION)
		message(WARNING "Required CLN ${CLN_FIND_VERSION} but found only CLN ${CLN_VERSION}.")
		return()
	endif()
endif()

# Cleanup
mark_as_advanced(
    CLN_FOUND
    CLN_INCLUDE_DIR
    CLN_LIBRARY
	CLN_VERSION
)
