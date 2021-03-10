# locate header
FIND_PATH(FMT_INCLUDE_DIR "printf.h"
    PATHS 
        /usr/include
        /opt/gnome/include
        /opt/openwin/include
        /sw/include
        /opt/local/include
	    ${DEPENDENCIES_DIR}/FMT
	    $ENV{FMT_ROOT_DIR}
    PATH_SUFFIXES
        fmt
)

FIND_LIBRARY(FMT_LIBRARY "fmt" "fmtd"
    PATHS
        /usr/local/lib64
        /usr/local/lib
        # fix for Ubuntu == 11.04 (Natty Narwhal)
        /usr/lib/i386-linux-gnu/
        /usr/lib/x86_64-linux-gnu/
        # end
        # fix for Ubuntu >= 11.10 (Oneiric Ocelot)
        /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
        # end
        /usr/lib64
        /usr/lib
        /opt/gnome/lib
        /usr/openwin/lib
        /sw/lib
        /opt/local/lib
	    ${DEPENDENCIES_DIR}/FMT
	    $ENV{FMT_ROOT_DIR}
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FMT DEFAULT_MSG
    FMT_INCLUDE_DIR FMT_LIBRARY)

IF(FMT_FOUND)
	MARK_AS_ADVANCED(FMT_INCLUDE_DIR FMT_LIBRARY)

     add_library(fmt::fmt UNKNOWN IMPORTED)
    set_target_properties(
      fmt::fmt PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${FMT_INCLUDE_DIR}"
    )
    set_target_properties(
      fmt::fmt PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${FMT_LIBRARY}"
    )
ENDIF(FMT_FOUND)
