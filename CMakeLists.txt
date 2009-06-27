# juffed CMakeLists.txt
# 
#
# To build juffed, you will need cmake 2.4.3 (should also work with 2.4.0 and above
# but not tested). To build the application run this commands:
# 
#	mkdir cbuild
#	cd cbuild
#	cmake ../
#	make
# 
# If you don't know cmake, here are some links for you:
# 	http://wiki.qtcentre.org/index.php?title=Compiling_Qt4_apps_with_CMake
# 	http://qtnode.net/wiki/Qt_with_cmake#cmake_for_Qt_4

PROJECT (juffed)
CMAKE_MINIMUM_REQUIRED (VERSION 2.4.0)
    IF (COMMAND cmake_policy)
      CMAKE_POLICY (SET CMP0003 NEW)
    ENDIF (COMMAND cmake_policy)

SET ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

SET ( JUFFED "juffed" CACHE STRING "Name of the executeable" )


#Version
FILE ( READ "${CMAKE_SOURCE_DIR}/version" JUFFED_VERSION )
STRING ( STRIP "${JUFFED_VERSION}" JUFFED_VERSION )

# flag indicating that this is a development version
SET ( DEV 1 )

IF ( DEV )
	# SVN revision
	SET ( ENV{LC_MESSAGES} "C" )
	EXECUTE_PROCESS ( 
		COMMAND svn info ${CMAKE_SOURCE_DIR}
		COMMAND grep Revision
		COMMAND cut -d: -f2 
		OUTPUT_VARIABLE REV
	)
	STRING ( STRIP "${REV}" REV )

	SET ( JUFFED_VERSION "${JUFFED_VERSION}.${REV}" )
ENDIF ( DEV )

SET ( VERSION ${JUFFED_VERSION} )
MESSAGE ( "${JUFFED} version: ${VERSION}" )

# Install Directories
SET(BIN_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/bin"
    CACHE PATH "Install path for executables"
)

SET(DATA_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}"
    CACHE PATH "Install path for shared data"
)

# verbose is off by default
#SET( CMAKE_VERBOSE_MAKEFILE  ON )

# input files

SET ( libjuffed_SRCS
	src/lib/AutocompleteSettings.cpp
	src/lib/CharsetsSettings.cpp
	src/lib/CommandStorage.cpp
	src/lib/FileTypeSettings.cpp
	src/lib/IconManager.cpp
	src/lib/Log.cpp
	src/lib/MainSettings.cpp
	src/lib/PluginSettings.cpp
	src/lib/Settings.cpp
	src/lib/TextDocSettings.cpp
)

SET ( juffed_UIS
	src/gui/AutocompleteSettingsPage.ui
	src/gui/CharsetsSettingsPage.ui
	src/gui/EditorSettingsPage.ui
	src/gui/FileTypesPage.ui
	src/gui/FindDlg.ui
	src/gui/MainSettingsPage.ui
	src/gui/SessionDlg.ui
	src/gui/ViewSettingsPage.ui
)

SET ( juffed_SRCS
	src/DocHandler.cpp
	src/Document.cpp
	src/JuffApp.cpp
	src/LexerStorage.cpp
	src/main.cpp
	src/Manager.cpp
	src/JuffScintilla.cpp
	src/NullDoc.cpp
	src/PluginManager.cpp
	src/RichDoc.cpp
	src/RichDocHandler.cpp
	src/SciDoc.cpp
	src/SciDocHandler.cpp
	src/SimpleDoc.cpp
	src/SimpleDocHandler.cpp
	src/SocketListener.cpp
	src/gui/AboutDlg.cpp
	src/gui/CharsetsSettingsPage.cpp
	src/gui/ColorButton.cpp
	src/gui/FileTypesPage.cpp
	src/gui/FindDlg.cpp
	src/gui/GUI.cpp
	src/gui/GUIManager.cpp
	src/gui/MultiPage.cpp
	src/gui/MW.cpp
	src/gui/PluginPage.cpp
	src/gui/SessionDlg.cpp
	src/gui/SettingsDlg.cpp
	src/gui/StatusLabel.cpp
	src/gui/TabBar.cpp
	src/gui/TabWidget.cpp
	src/gui/Viewer.cpp
)

SET ( juffed_MOC_HDRS
	src/DocHandler.h
	src/Document.h
	src/JuffApp.h
	src/Manager.h
	src/JuffScintilla.h
	src/PluginManager.h
	src/RichDoc.h
	src/RichDocHandler.h
	src/SciDoc.h
	src/SciDocHandler.h
	src/SocketListener.h
	src/gui/AboutDlg.h
	src/gui/CharsetsSettingsPage.h
	src/gui/ColorButton.h
	src/gui/FileTypesPage.h
	src/gui/FindDlg.h
	src/gui/GUI.h
	src/gui/MultiPage.h
	src/gui/MW.h
	src/gui/PluginPage.h
	src/gui/SessionDlg.h
	src/gui/SettingsDlg.h
	src/gui/TabBar.h
	src/gui/TabWidget.h
	src/gui/Viewer.h
)

SET ( juffed_RCS
	src/juffed.rc
)

SET ( juffed_RESOURCES
	src/juffed.qrc
)

SET ( juffed_TS
	l10n/juffed_de.ts
	l10n/juffed_fr.ts
	l10n/juffed_pl.ts
	l10n/juffed_pt.ts
	l10n/juffed_ru.ts
	l10n/juffed_sp.ts
	l10n/juffed_zh_CN.ts
)

SET ( juffed_includes
	include/AppInfo.h
	include/AutocompleteSettings.h
	include/CharsetsSettings.h
	include/CommandStorage.h
	include/Functions.h
	include/IconManager.h
	include/Juff.h
	include/JuffPlugin.h
	include/License.h
	include/Log.h
	include/MainSettings.h
	include/ManagerInterface.h
	include/Parameter.h
	include/PluginSettings.h
	include/Settings.h
	include/TextDocSettings.h
)

FIND_PACKAGE ( Qt4 REQUIRED )
FIND_PACKAGE ( QScintilla2 REQUIRED)
SET (QT_USE_QTXML TRUE)
INCLUDE (${QT_USE_FILE})

#Configure AppInfo.h
configure_file ("${CMAKE_SOURCE_DIR}/src/AppInfo.nix.h.cmake" "${CMAKE_BINARY_DIR}/AppInfo.nix.h" @ONLY)
configure_file ("${CMAKE_SOURCE_DIR}/cmake/version.cmake" "${CMAKE_BINARY_DIR}/version.real" @ONLY)

# qt4 actions
QT4_WRAP_UI ( juffed_UIS_H ${juffed_UIS} )
QT4_WRAP_CPP ( juffed_MOC_SRCS ${juffed_MOC_HDRS} )
QT4_ADD_RESOURCES ( juffed_RESOURCES ${juffed_RESOURCES} )
QT4_AUTOMOC ( ${juffed_SRCS} )
QT4_ADD_TRANSLATION ( juffed_QM ${juffed_TS} )

# definitions for the project
ADD_DEFINITIONS(
	-Wall 
#	-DQT_NO_DEBUG
	-DJUFF_DEBUG
	-DJUFF_FULL_DEBUG
	${QT_DEFINITIONS}
)

# find TCL lexer
find_path ( QSCI_TCL_LEXER qscilexertcl.h ${LIBQSCINTILLA2_INCLUDE_DIR} )
if ( QSCI_TCL_LEXER )
	ADD_DEFINITIONS (
		-DJUFF_TCL_LEXER
	)
endif ( QSCI_TCL_LEXER )

# find Pascal lexer
find_path ( QSCI_PASCAL_LEXER qscilexerpascal.h ${LIBQSCINTILLA2_INCLUDE_DIR} )
if ( QSCI_PASCAL_LEXER )
	ADD_DEFINITIONS (
		-DJUFF_PASCAL_LEXER
	)
endif ( QSCI_PASCAL_LEXER )

# include directories
INCLUDE_DIRECTORIES(
	${QT_INCLUDES}
	${CMAKE_BINARY_DIR}
	${LIBQSCINTILLA2_INCLUDE_DIR}
	src
	./include
)


IF ( MINGW )
	ADD_DEFINITIONS(
		-DQSCINTILLA_DLL
	)
	# resource compilation for MinGW
	ADD_CUSTOM_COMMAND ( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/juffed_ico.o
		COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}/src -i${CMAKE_CURRENT_SOURCE_DIR}/src/juffed.rc 
			-o ${CMAKE_CURRENT_BINARY_DIR}/src/juffed_ico.o )
	SET ( juffed_SRCS ${juffed_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/src/juffed_ico.o)
ELSE ( MINGW )
	SET(juffed_SRCS	${juffed_SRCS})
ENDIF ( MINGW )

ADD_LIBRARY ( juff SHARED
	${libjuffed_SRCS}
)

# the main build target
ADD_EXECUTABLE ( ${JUFFED}
	${juffed_SRCS} 
	${juffed_UIS_H} 
	${juffed_MOC_SRCS} 
	${juffed_UIS} 
	${juffed_RESOURCES} 
	${juffed_QM} 
)

IF ( MINGW )
	SET_TARGET_PROPERTIES( ${JUFFED} PROPERTIES LINK_FLAGS "-mwindows" )
ENDIF ( MINGW )

TARGET_LINK_LIBRARIES ( juff
	${QT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

TARGET_LINK_LIBRARIES ( ${JUFFED} juff
	${QT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTXML_LIBRARY}
	${LIBQSCINTILLA2_LIBRARY} 
)

INSTALL ( TARGETS juffed DESTINATION ${BIN_INSTALL_DIR})
INSTALL ( TARGETS juff DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
INSTALL ( FILES ${juffed_QM} DESTINATION ${DATA_INSTALL_DIR}/l10n )
INSTALL ( DIRECTORY hlschemes DESTINATION ${DATA_INSTALL_DIR} PATTERN ".svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
INSTALL ( DIRECTORY apis DESTINATION ${DATA_INSTALL_DIR} PATTERN "*.svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
INSTALL ( FILES juffed.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )
INSTALL ( FILES juffed.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
INSTALL ( FILES ${juffed_includes} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${CMAKE_PROJECT_NAME} )

# uninstall 
CONFIGURE_FILE(
"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
