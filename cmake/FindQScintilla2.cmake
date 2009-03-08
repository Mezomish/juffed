# - Try to find libqscintilla2
# Once done this will define
#
#  LIBQSCINTILLA2_FOUND - system has libqscintilla2
#  LIBQSCINTILLA2_INCLUDE_DIR - the libqscintilla2 include directory
#  LIBQSCINTILLA2_LIBRARY - Link this to use libqscintilla2
#
# based on FindLibArchive.cmake by Pino Toscano, <toscano.pino@tiscali.it>
# Copyright (c) 2008, David Stegbauer, <daaste@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

FIND_PACKAGE( Qt4 REQUIRED )

# QT4_FOUND
# QT_INCLUDE_DIR /Qsci
# QT_LIBRARY_DIR

if (QT4_FOUND)
    include(CheckLibraryExists)

    if (LIBQSCINTILLA2_LIBRARY AND LIBQSCINTILLA2_INCLUDE_DIR)
        # in cache already
        set(LIBQSCINTILLA2_FOUND TRUE)
    else (LIBQSCINTILLA2_LIBRARY AND LIBQSCINTILLA2_INCLUDE_DIR)

        find_path(LIBQSCINTILLA2_INCLUDE_DIR qsciscintilla.h
            PATHS
            ${QT_INCLUDE_DIR}/Qsci
            ${CMAKE_INSTALL_PREFIX}/include/Qsci
        )

        find_library(LIBQSCINTILLA2_LIBRARY NAMES qscintilla2 libqscintilla2
            PATHS
            ${QT_LIBRARY_DIR}
            ${CMAKE_INSTALL_PREFIX}/lib
        )

        include(FindPackageHandleStandardArgs)
        FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibQScintilla2 DEFAULT_MSG LIBQSCINTILLA2_INCLUDE_DIR LIBQSCINTILLA2_LIBRARY )

        # ensure that they are cached
        set(LIBQSCINTILLA2_INCLUDE_DIR ${LIBQSCINTILLA2_INCLUDE_DIR} CACHE INTERNAL "The libqscintilla2 include path")
        set(LIBQSCINTILLA2_LIBRARY ${LIBQSCINTILLA2_LIBRARY} CACHE INTERNAL "The libraries needed to use libqscintilla2")

    endif (LIBQSCINTILLA2_LIBRARY AND LIBQSCINTILLA2_INCLUDE_DIR)
else (QT4_FOUND)
    # Qt4 not found, qscintilla2 unusable even if found,
    # so report no qscintilla2
    set(LIBQSCINTILLA2_FOUND FALSE)
endif (QT4_FOUND)
