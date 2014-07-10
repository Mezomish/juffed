# - Try to find the QtSingleApplication includes and library
# which defines
#
# QTSINGLEAPPLICATION_FOUND - system has QtSingleApplication
# QTSINGLEAPPLICATION_INCLUDE_DIR - where to find header QtSingleApplication
# QTSINGLEAPPLICATION_LIBRARIES - the libraries to link against to use QtSingleApplication
# QTSINGLEAPPLICATION_LIBRARY - where to find the QtSingleApplication library (not for general use)

# copyright (c) 2013 TI_Eugene ti.eugene@gmail.com
#
# Redistribution and use is allowed according to the terms of the FreeBSD license.

SET(QTSINGLEAPPLICATION_FOUND FALSE)

IF(QT4_FOUND)
    FIND_PATH(QTSINGLEAPPLICATION_INCLUDE_DIR QtSingleApplication
                # standard locations
                /usr/include
                /usr/include/QtSolutions
                # qt4 location except mac's frameworks
                "${QT_INCLUDE_DIR}/QtSolutions"
                # mac's frameworks
                ${FRAMEWORK_INCLUDE_DIR}/QtSolutions
    )

    SET(QTSINGLEAPPLICATION_NAMES ${QTSINGLEAPPLICATION_NAMES} QtSolutions_SingleApplication-2.6 libQtSolutions_SingleApplication-2.6)
    FIND_LIBRARY(QTSINGLEAPPLICATION_LIBRARY
        NAMES ${QTSINGLEAPPLICATION_NAMES}
        PATHS ${QT_LIBRARY_DIR}
    )
ELSEIF(Qt5Widgets_FOUND)
    FOREACH(TOP_INCLUDE_PATH in ${Qt5Widgets_INCLUDE_DIRS} ${FRAMEWORK_INCLUDE_DIR})
        FIND_PATH(QTSINGLEAPPLICATION_INCLUDE_DIR QtSingleApplication ${TOP_INCLUDE_PATH}/QtSolutions)

        IF(QTSINGLEAPPLICATION_INCLUDE_DIR)
            BREAK()
        ENDIF()
    ENDFOREACH()

    SET(QTSINGLEAPPLICATION_NAMES ${QTSINGLEAPPLICATION_NAMES} QtSolutions_SingleApplication-2.6 libQtSolutions_SingleApplication-2.6)
    GET_TARGET_PROPERTY(QT5_WIDGETSLIBRARY Qt5::Widgets LOCATION)
    GET_FILENAME_COMPONENT(QT5_WIDGETSLIBRARYPATH ${QT5_WIDGETSLIBRARY} PATH)

    FIND_LIBRARY(QTSINGLEAPPLICATION_LIBRARY
        NAMES ${QTSINGLEAPPLICATION_NAMES}
        PATHS ${QT5_WIDGETSLIBRARYPATH}
    )
ENDIF()

IF (QTSINGLEAPPLICATION_LIBRARY AND QTSINGLEAPPLICATION_INCLUDE_DIR)

    SET(QTSINGLEAPPLICATION_LIBRARIES ${QTSINGLEAPPLICATION_LIBRARY})
    SET(QTSINGLEAPPLICATION_FOUND TRUE)

    IF (CYGWIN)
        IF(BUILD_SHARED_LIBS)
        # No need to define QTSINGLEAPPLICATION_USE_DLL here, because it's default for Cygwin.
        ELSE(BUILD_SHARED_LIBS)
        SET (QTSINGLEAPPLICATION_DEFINITIONS -DQTSINGLEAPPLICATION_STATIC)
        ENDIF(BUILD_SHARED_LIBS)
    ENDIF (CYGWIN)

ENDIF (QTSINGLEAPPLICATION_LIBRARY AND QTSINGLEAPPLICATION_INCLUDE_DIR)

IF (QTSINGLEAPPLICATION_FOUND)
  IF (NOT QtSingleApplication_FIND_QUIETLY)
    MESSAGE(STATUS "Found QtSingleApplication: ${QTSINGLEAPPLICATION_LIBRARY}")
    MESSAGE(STATUS "         includes: ${QTSINGLEAPPLICATION_INCLUDE_DIR}")
  ENDIF (NOT QtSingleApplication_FIND_QUIETLY)
ELSE (QTSINGLEAPPLICATION_FOUND)
  IF (QtSingleApplication_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find QtSingleApplication library")
  ENDIF (QtSingleApplication_FIND_REQUIRED)
ENDIF (QTSINGLEAPPLICATION_FOUND)

MARK_AS_ADVANCED(QTSINGLEAPPLICATION_INCLUDE_DIR QTSINGLEAPPLICATION_LIBRARY)

