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

SET ( JUFFED "juffed" CACHE STRING "Name of the executeable" )
SET ( JUFFLIB "juff" CACHE STRING "Name of the library" )


#Version
FILE ( READ "${CMAKE_SOURCE_DIR}/base_version" BASE_VERSION )
FILE ( READ "${CMAKE_SOURCE_DIR}/version"      JUFFED_VERSION )
STRING ( STRIP "${BASE_VERSION}"   BASE_VERSION )
STRING ( STRIP "${JUFFED_VERSION}" JUFFED_VERSION )

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

#SET ( VERSION ${JUFFED_VERSION} )
MESSAGE ( "${JUFFED} version: ${JUFFED_VERSION}" )

# Install Directories
SET (BIN_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/bin"
    CACHE PATH "Install path for executables"
)

SET (LIB_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/lib"
    CACHE PATH "Install path for libraries"
)

SET (DATA_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}"
    CACHE PATH "Install path for shared data"
)

SET (CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}")


###############
# File sets

SET ( juffed_lib_MOC_HEADERS
	include/Document.h
	include/JuffAPI.h
	include/Project.h
	include/PluginNotifier.h
)

SET ( juffed_lib_SRCS
	src/lib/AutocompleteSettings.cpp
	src/lib/CharsetSettings.cpp
	src/lib/EditorSettings.cpp
	src/lib/FileTypeSettings.cpp
	src/lib/KeySettings.cpp
	src/lib/MainSettings.cpp
	src/lib/PluginSettings.cpp
	src/lib/Settings.cpp
	src/lib/QSciSettings.cpp
	
	src/lib/CommandStorage.cpp
	src/lib/Document.cpp
	src/lib/DocEngine.cpp
	src/lib/Functions.cpp
	src/lib/IconManager.cpp
	src/lib/JuffAPI.cpp
	src/lib/Log.cpp
	src/lib/NullDoc.cpp
	src/lib/Project.cpp
	src/lib/PluginNotifier.cpp
)

SET ( juffed_lib_RESOURCES
	src/lib/LibResources.qrc
)

SET ( juffed_app_MOC_HEADERS
	src/app/JuffEd.h
	src/app/SearchEngine.h
	src/app/qsci/JuffScintilla.h
	src/app/qsci/SciDoc.h
	src/app/qsci/SciDocEngine.h
	src/app/ui/AboutDlg.h
	src/app/ui/DocViewer.h
	src/app/ui/FindDlg.h
	src/app/ui/JuffMW.h
	src/app/ui/JumpToFileDlg.h
	src/app/ui/MessageWidget.h
	src/app/ui/SelectFilesDlg.h
	src/app/ui/StatusLabel.h
	src/app/ui/TabBar.h
	src/app/ui/TabWidget.h
	src/3rd_party/qtsingleapplication/qtsinglecoreapplication.h
	src/3rd_party/qtsingleapplication/qtsingleapplication.h
	src/3rd_party/qtsingleapplication/qtlocalpeer.h
	
	# settings
	src/app/ui/settings/CharsetsSettingsPage.h
	src/app/ui/settings/ColorButton.h
#	src/app/ui/settings/FileTypesPage.h
	src/app/ui/settings/MultiPage.h
	src/app/ui/settings/PluginPage.h
	src/app/ui/settings/SettingsDlg.h
	src/app/ui/settings/SettingsCheckItem.h
	src/app/ui/settings/SettingsItem.h
	src/app/ui/settings/SettingsPage.h
	src/app/ui/settings/SettingsSelectItem.h
)

SET ( juffed_app_SRCS
	src/app/DocManager.cpp
	src/app/JuffEd.cpp
	src/app/main.cpp
	src/app/PluginManager.cpp
	src/app/SearchEngine.cpp
	src/app/qsci/JuffScintilla.cpp
	src/app/qsci/LexerStorage.cpp
	src/app/qsci/SciDoc.cpp
	src/app/qsci/SciDocEngine.cpp
	src/app/ui/AboutDlg.cpp
	src/app/ui/DocViewer.cpp
	src/app/ui/FindDlg.cpp
	src/app/ui/JuffMW.cpp
	src/app/ui/JumpToFileDlg.cpp
	src/app/ui/MessageWidget.cpp
	src/app/ui/SelectFilesDlg.cpp
	src/app/ui/StatusLabel.cpp
	src/app/ui/TabBar.cpp
	src/app/ui/TabWidget.cpp
	src/3rd_party/qtsingleapplication/qtsinglecoreapplication.cpp
	src/3rd_party/qtsingleapplication/qtsingleapplication.cpp
	src/3rd_party/qtsingleapplication/qtlockedfile.cpp
	src/3rd_party/qtsingleapplication/qtlocalpeer.cpp
	
	# settings
	src/app/ui/settings/CharsetsSettingsPage.cpp
	src/app/ui/settings/ColorButton.cpp
#	src/app/ui/settings/FileTypesPage.cpp
	src/app/ui/settings/MultiPage.cpp
	src/app/ui/settings/PluginPage.cpp
	src/app/ui/settings/SettingsDlg.cpp
	src/app/ui/settings/SettingsCheckItem.cpp
	src/app/ui/settings/SettingsItem.cpp
	src/app/ui/settings/SettingsPage.cpp
	src/app/ui/settings/SettingsSelectItem.cpp
)

IF ( WIN32 )
	SET ( juffed_app_SRCS
		${juffed_app_SRCS}
		src/3rd_party/qtsingleapplication/qtlockedfile_win.cpp
	)
ENDIF ( WIN32 )

IF ( UNIX )
	SET ( juffed_app_SRCS
		${juffed_app_SRCS}
		src/3rd_party/qtsingleapplication/qtlockedfile_unix.cpp
	)
ENDIF ( UNIX )

SET ( juffed_app_UIS
	src/app/ui/forms/FindDlg.ui
	src/app/ui/forms/JumpToFileDlg.ui
	src/app/ui/forms/MessageWidget.ui
	src/app/ui/forms/SelectFilesDlg.ui
	
	# settings
	src/app/ui/forms/AutocompleteSettingsPage.ui
	src/app/ui/forms/CharsetsSettingsPage.ui
	src/app/ui/forms/EditorSettingsPage.ui
	src/app/ui/forms/FileTypesPage.ui
	src/app/ui/forms/MainSettingsPage.ui
	src/app/ui/forms/SessionDlg.ui
	src/app/ui/forms/ViewSettingsPage.ui
	src/app/qsci/forms/QSciSettings.ui
)

SET ( juffed_app_RESOURCES
	src/app/Resources.qrc
)

SET ( juffed_TS
	l10n/juffed_de.ts
	l10n/juffed_fr.ts
	l10n/juffed_pl.ts
	l10n/juffed_pt.ts
	l10n/juffed_ru.ts
	l10n/juffed_sp.ts
	l10n/juffed_zh.ts
	l10n/juffed_cs.ts
)

SET ( juffed_DEV_HEADERS
	include/AppInfo.h
	${CMAKE_BINARY_DIR}/AppInfo.win.h
	${CMAKE_BINARY_DIR}/AppInfo.nix.h
	include/AutocompleteSettings.h
	include/CharsetSettings.h
	include/CommandStorage.h
	include/Document.h
	include/DocEngine.h
	include/DocHandlerInt.h
	include/EditorSettings.h
	include/Enums.h
	include/FileTypeSettings.h
	include/Functions.h
	include/IconManager.h
	include/JuffAPI.h
	include/JuffPlugin.h
	include/KeySettings.h
	include/Log.h
	include/MainSettings.h
	include/NullDoc.h
#	include/PluginNotifier.h
	include/PluginSettings.h
	include/Project.h
	include/QSciSettings.h
	include/Types.h
	include/Settings.h
)

FIND_PACKAGE ( Qt4 REQUIRED )
FIND_PACKAGE ( QScintilla2 REQUIRED)

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

ADD_LIBRARY ( ${JUFFLIB} SHARED
	${juffed_lib_SRCS}
	${juffed_lib_MOC_SRCS}
	${juffed_lib_RESOURCES} 
)

SET_TARGET_PROPERTIES ( ${JUFFLIB} PROPERTIES VERSION ${JUFFED_VERSION} SOVERSION ${BASE_VERSION} )

# the main build target
ADD_EXECUTABLE ( ${JUFFED}
	${juffed_app_SRCS} 
	${juffed_app_MOC_SRCS} 
	${juffed_app_UIS_H}
	${juffed_app_RESOURCES} 
	${juffed_QM} 
)

TARGET_LINK_LIBRARIES ( ${JUFFLIB}
	${QT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTXML_LIBRARY}
)

TARGET_LINK_LIBRARIES ( ${JUFFED} ${JUFFLIB}
	${QT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTXML_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${LIBQSCINTILLA2_LIBRARY} 
)




# verbose is off by default
SET ( CMAKE_VERBOSE_MAKEFILE  ON )


#SET ( juffed_RCS
#	src/juffed.rc
#)

#SET (QT_USE_QTXML TRUE)
#INCLUDE (${QT_USE_FILE})

#Configure AppInfo.h
configure_file ("${CMAKE_SOURCE_DIR}/cmake/AppInfo.nix.h.cmake" "${CMAKE_BINARY_DIR}/AppInfo.nix.h" @ONLY)
configure_file ("${CMAKE_SOURCE_DIR}/cmake/AppInfo.win.h.cmake" "${CMAKE_BINARY_DIR}/AppInfo.win.h" @ONLY)
#configure_file ("${CMAKE_SOURCE_DIR}/cmake/version.cmake" "${CMAKE_BINARY_DIR}/version.real" @ONLY)

# find TCL lexer
#find_path ( QSCI_TCL_LEXER qscilexertcl.h ${LIBQSCINTILLA2_INCLUDE_DIR} )
#if ( QSCI_TCL_LEXER )
#	ADD_DEFINITIONS (
#		-DJUFF_TCL_LEXER
#	)
#endif ( QSCI_TCL_LEXER )

# find Pascal lexer
#find_path ( QSCI_PASCAL_LEXER qscilexerpascal.h ${LIBQSCINTILLA2_INCLUDE_DIR} )
#if ( QSCI_PASCAL_LEXER )
#	ADD_DEFINITIONS (
#		-DJUFF_PASCAL_LEXER
#	)
#endif ( QSCI_PASCAL_LEXER )

# find Fortran lexer
#find_path ( QSCI_FORTRAN_LEXER qscilexerfortran.h ${LIBQSCINTILLA2_INCLUDE_DIR} )
#if ( QSCI_FORTRAN_LEXER )
#	ADD_DEFINITIONS (
#		-DJUFF_FORTRAN_LEXER
#	)
#endif ( QSCI_FORTRAN_LEXER )

# find CSS3Property
#find_path ( QSCI_CSS_LEXER qscilexercss.h ${LIBQSCINTILLA2_INCLUDE_DIR} )
#if ( QSCI_CSS_LEXER )
#	EXECUTE_PROCESS ( 
#		COMMAND grep CSS3Property ${QSCI_CSS_LEXER}/qscilexercss.h
#		OUTPUT_VARIABLE CSS3_FOUND
#	)
#	if ( CSS3_FOUND )
#		MESSAGE ( "CSS3 found!" )
#		ADD_DEFINITIONS (
#			-DCSS3_FOUND
#		)
#	endif ( CSS3_FOUND )
#endif (QSCI_CSS_LEXER )

# include directories
#INCLUDE_DIRECTORIES(
#	${QT_INCLUDES}
#	${CMAKE_BINARY_DIR}
#	${LIBQSCINTILLA2_INCLUDE_DIR}
#	src
#	./include
#)


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

#ADD_LIBRARY ( juff SHARED
#	${libjuffed_SRCS}
#	${libjuffed_MOC_SRCS}
#)

#SET_TARGET_PROPERTIES ( juff PROPERTIES VERSION ${VERSION} SOVERSION "0.7")

# the main build target
#ADD_EXECUTABLE ( ${JUFFED}
#	${juffed_SRCS} 
#	${juffed_UIS_H} 
#	${juffed_MOC_SRCS} 
#	${juffed_UIS} 
#	${juffed_RESOURCES} 
#	${juffed_QM} 
#)
#
#IF ( MINGW )
#	SET_TARGET_PROPERTIES( ${JUFFED} PROPERTIES LINK_FLAGS "-mwindows" )
#ENDIF ( MINGW )
#
#TARGET_LINK_LIBRARIES ( juff
#	${QT_LIBRARIES}
#	${QT_QTCORE_LIBRARY}
#	${QT_QTGUI_LIBRARY}
#)
#
#TARGET_LINK_LIBRARIES ( ${JUFFED} juff
#	${QT_LIBRARIES}
#	${QT_QTCORE_LIBRARY}
#	${QT_QTGUI_LIBRARY}
#	${QT_QTXML_LIBRARY}
#	${QT_QTNETWORK_LIBRARY}
#	${LIBQSCINTILLA2_LIBRARY} 
#)


INSTALL ( TARGETS ${JUFFED} DESTINATION ${BIN_INSTALL_DIR} )
INSTALL ( TARGETS ${JUFFLIB} DESTINATION ${LIB_INSTALL_DIR} )

INSTALL ( FILES ${juffed_QM} DESTINATION ${DATA_INSTALL_DIR}/l10n )
INSTALL ( DIRECTORY hlschemes DESTINATION ${DATA_INSTALL_DIR} PATTERN ".svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
INSTALL ( DIRECTORY apis DESTINATION ${DATA_INSTALL_DIR} PATTERN "*.svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
INSTALL ( FILES juffed.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )
INSTALL ( FILES juffed.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
INSTALL ( FILES ${juffed_DEV_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${CMAKE_PROJECT_NAME} )
#
## uninstall 
CONFIGURE_FILE ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )

ADD_CUSTOM_TARGET ( uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
