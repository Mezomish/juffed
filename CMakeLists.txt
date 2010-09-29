# juffed CMakeLists.txt
# 
# To build juffed, you will need cmake 2.6.0.
# To build the application run this commands:
# 
#	mkdir build
#	cd build
#	cmake .. -DCMAKE_INSTALL_PREFIX=/usr (or whatever prefix you'd like)
#	make

PROJECT (juffed)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6.0)
    IF (COMMAND cmake_policy)
      CMAKE_POLICY (SET CMP0003 NEW)
    ENDIF (COMMAND cmake_policy)

SET ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )
SET ( PREFIX ${CMAKE_INSTALL_PREFIX} )

SET ( CMAKE_VERBOSE_MAKEFILE OFF )

####################
# Names and Version
####################

SET ( JUFFED  "juffed" CACHE STRING "Name of the executeable" )
SET ( JUFFLIB "juff"   CACHE STRING "Name of the library" )
FILE   ( READ "${CMAKE_SOURCE_DIR}/version"      JUFFED_VERSION )
FILE   ( READ "${CMAKE_SOURCE_DIR}/base_version" BASE_VERSION )
STRING ( STRIP "${JUFFED_VERSION}" JUFFED_VERSION )
STRING ( STRIP "${BASE_VERSION}"   BASE_VERSION )

# flag indicating that this is a development version
SET ( DEV 1 )

IF ( DEV )
	# SVN revision
	FIND_PACKAGE(Subversion)
	IF(Subversion_FOUND)
		Subversion_WC_INFO(${PROJECT_SOURCE_DIR} SVN)
	ENDIF(Subversion_FOUND)
	SET ( REV ${SVN_WC_REVISION} )
	SET ( JUFFED_VERSION "${BASE_VERSION}.${REV}" )
ENDIF ( DEV )

MESSAGE ( "${JUFFED} version: ${JUFFED_VERSION}" )


####################
# Directories
####################

SET (BIN_INSTALL_DIR
    "${PREFIX}/bin"
    CACHE PATH "Install path for executables"
)

SET (LIB_INSTALL_DIR
    "${PREFIX}/lib"
    CACHE PATH "Install path for libraries"
)

SET (DATA_INSTALL_DIR
    "${PREFIX}/share/${CMAKE_PROJECT_NAME}"
    CACHE PATH "Install path for shared data"
)

#SET (CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}")



FIND_PACKAGE ( Qt4 REQUIRED )
FIND_PACKAGE ( QScintilla2 REQUIRED)

INCLUDE ( cmake/FileSets.cmake )
INCLUDE ( cmake/SomeLexers.cmake )

QT4_WRAP_CPP ( juffed_lib_MOC_SRCS ${juffed_lib_MOC_HEADERS} )
QT4_WRAP_CPP ( juffed_app_MOC_SRCS ${juffed_app_MOC_HEADERS} )
QT4_ADD_RESOURCES ( juffed_app_RESOURCES ${juffed_app_RESOURCES} )
QT4_ADD_RESOURCES ( juffed_lib_RESOURCES ${juffed_lib_RESOURCES} )
QT4_AUTOMOC ( ${juffed_lib_SRCS} )
QT4_AUTOMOC ( ${juffed_app_SRCS} )
QT4_WRAP_UI ( juffed_app_UIS_H ${juffed_app_UIS} )
QT4_ADD_TRANSLATION ( juffed_QM ${juffed_TS} )

# include directories
INCLUDE_DIRECTORIES(
	${QT_INCLUDES}
	${CMAKE_BINARY_DIR}
	${LIBQSCINTILLA2_INCLUDE_DIR}
	src/app/ui
	src/app/qsci
	./include
)

configure_file ("${CMAKE_SOURCE_DIR}/cmake/AppInfo.nix.h.cmake" "${CMAKE_BINARY_DIR}/AppInfo.nix.h" @ONLY)
configure_file ("${CMAKE_SOURCE_DIR}/cmake/AppInfo.win.h.cmake" "${CMAKE_BINARY_DIR}/AppInfo.win.h" @ONLY)


####################
# Targets
####################

# library
ADD_LIBRARY ( ${JUFFLIB} SHARED
	${juffed_lib_SRCS}
	${juffed_lib_MOC_SRCS}
	${juffed_lib_RESOURCES} 
)
TARGET_LINK_LIBRARIES ( ${JUFFLIB}
	${QT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTXML_LIBRARY}
)
SET_TARGET_PROPERTIES ( ${JUFFLIB} PROPERTIES VERSION ${JUFFED_VERSION} SOVERSION ${BASE_VERSION} )

# executable
ADD_EXECUTABLE ( ${JUFFED}
	${juffed_app_SRCS} 
	${juffed_app_MOC_SRCS} 
	${juffed_UIS} 
	${juffed_app_UIS_H}
	${juffed_app_RESOURCES} 
	${juffed_QM} 
)
TARGET_LINK_LIBRARIES ( ${JUFFED} ${JUFFLIB}
	${QT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTXML_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${LIBQSCINTILLA2_LIBRARY} 
)





#SET ( juffed_RCS
#	src/juffed.rc
#)

#SET (QT_USE_QTXML TRUE)
#INCLUDE (${QT_USE_FILE})

#configure_file ("${CMAKE_SOURCE_DIR}/cmake/version.cmake" "${CMAKE_BINARY_DIR}/version.real" @ONLY)


#IF ( MINGW )
#	ADD_DEFINITIONS(
#		-DQSCINTILLA_DLL
#	)
	# resource compilation for MinGW
#	ADD_CUSTOM_COMMAND ( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/juffed_ico.o
#		COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}/src -i${CMAKE_CURRENT_SOURCE_DIR}/src/juffed.rc 
#			-o ${CMAKE_CURRENT_BINARY_DIR}/src/juffed_ico.o )
#	SET ( juffed_SRCS ${juffed_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/src/juffed_ico.o)
#ELSE ( MINGW )
#	SET(juffed_SRCS	${juffed_SRCS})
#ENDIF ( MINGW )

#IF ( MINGW )
#	SET_TARGET_PROPERTIES( ${JUFFED} PROPERTIES LINK_FLAGS "-mwindows" )
#ENDIF ( MINGW )
#

####################
# Installs
####################
INSTALL ( TARGETS ${JUFFED} DESTINATION ${BIN_INSTALL_DIR} )
INSTALL ( TARGETS ${JUFFLIB} DESTINATION ${LIB_INSTALL_DIR} )
INSTALL ( FILES ${juffed_QM} DESTINATION ${DATA_INSTALL_DIR}/l10n )
INSTALL ( DIRECTORY hlschemes DESTINATION ${DATA_INSTALL_DIR} PATTERN ".svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
INSTALL ( DIRECTORY apis DESTINATION ${DATA_INSTALL_DIR} PATTERN "*.svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
INSTALL ( FILES juffed.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )
INSTALL ( FILES juffed.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
INSTALL ( FILES ${juffed_DEV_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${CMAKE_PROJECT_NAME} )

## uninstall 
CONFIGURE_FILE ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )
ADD_CUSTOM_TARGET ( uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
