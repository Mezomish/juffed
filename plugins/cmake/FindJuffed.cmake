# Find Juffed editor's development stuff - for plugins
#
# This module defines
#  JUFFED_INCLUDE_DIR, where to find includes
#  JUFFED_LIBRARY, the libraries needed to use link with
#  JUFFED_FOUND, the flag id the system contains 
#
# Optional inputs:
# JUFFED_PATH_INCLUDES - cmake input variable to specify non-standard path to includes
# JUFFED_PATH_LIB - cmake input variable to specify non-standard path to library
#
# Petr Vanek <petr@scribus.info>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(JUFFED_INCLUDE_DIR  JuffPlugin.h
   ${JUFFED_PATH_INCLUDES}/
   ${CMAKE_INSTALL_PREFIX}/include/juffed
   /usr/local/include/
   /opt/local/include/
)

find_library(JUFFED_LIBRARY NAMES juff libjuff
    PATHS
        ${JUFFED_PATH_LIB}
        /usr/lib/
        /usr/lib${LIB_SUFFIX}/
        /usr/local/lib/
)

if (JUFFED_LIBRARY AND JUFFED_INCLUDE_DIR)
    set( JUFFED_FOUND 1 )
endif()


IF (JUFFED_FOUND)

    MESSAGE(STATUS "Juffed dev found")
    MESSAGE(STATUS " includes: ${JUFFED_INCLUDE_DIR}")
    MESSAGE(STATUS "     libs: ${JUFFED_LIBRARY}")

ELSE (JUFFED_FOUND)

    MESSAGE(STATUS "Juffed dev not found.")
    MESSAGE(STATUS "  You can specify includes: -DJUFFED_PATH_INCLUDES=/some/path")
    MESSAGE(STATUS "  currently found includes: ${JUFFED_INCLUDE_DIR}")
    MESSAGE(STATUS "      You can specify libs: -DJUFFED_PATH_LIB=/another/path")
    MESSAGE(STATUS "      currently found libs: ${JUFFED_LIBRARY}")

    IF (Juffed_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find Juffed dev")
    ENDIF ()

ENDIF (JUFFED_FOUND)

