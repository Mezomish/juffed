OPTION(BUILD_DOCLIST "Build doclist plugin" ON)
OPTION(BUILD_FAVORITES "Build favorites plugin" ON)
OPTION(BUILD_FINDFILES "Build findfiles plugin" ON)
OPTION(BUILD_FM "Build filemanager plugin" ON)
OPTION(BUILD_SYMBOLBROWSER "Build symbolbrowser plugin" ON)
OPTION(BUILD_XMLFORMAT "Build XML formatter plugin" ON)
OPTION(BUILD_SORT "Build Sort Document plugin" ON)
OPTION(BUILD_KEYS "Build Keybindings plugin" ON)
OPTION(BUILD_AUTOSAVE "Build Autosave plugin" ON)
OPTION(BUILD_COLORPICKER "Build ColorPicker plugin" ON)
OPTION(BUILD_COMPARE "Build Compare plugin" ON)
OPTION(BUILD_TERMINAL "Build terminal plugin" ON)

# disabled since it's really buggy and crashes on doc change
OPTION(BUILD_TODOLIST "Build TODOList plugin" OFF)

project (juffed-plugins)
cmake_minimum_required (VERSION 3.7)

option(USE_QT6 "Use Qt6. Defaults to Qt4" ON)
option(USE_QT5 "Use Qt5. Defaults to Qt4" OFF)

# plugins are built standalone if is JUFFED_CMAKE_PLUGINS not set
# Required variables (includes, libs) has to go from
# a) FindJuffed.cmake - for standalone build
# b) upstream CMakeLists.txt for the in-juffed build
set ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )
if (NOT JUFFED_CMAKE_PLUGINS)
    if (USE_QT6)
        find_package ( Qt6 COMPONENTS Widgets Xml Network PrintSupport LinguistTools CONFIG REQUIRED )
        set (JUFF_QT_IMPORTED_TARGETS Qt6::Widgets Qt6::Xml Qt6::Network Qt6::PrintSupport)

        macro(qt_wrap_ui)
          qt6_wrap_ui(${ARGN})
        endmacro()

        macro(qt_add_resources)
          qt6_add_resources(${ARGN})
        endmacro()

        macro(qt_add_translation)
          qt6_add_translation(${ARGN})
        endmacro()
    elseif (USE_QT5)
        find_package ( Qt5 COMPONENTS Widgets Xml Network PrintSupport LinguistTools CONFIG REQUIRED )
        set (JUFF_QT_IMPORTED_TARGETS Qt5::Widgets Qt5::Xml Qt5::Network Qt5::PrintSupport)

        macro(qt_wrap_ui)
          qt5_wrap_ui(${ARGN})
        endmacro()

        macro(qt_add_resources)
          qt5_add_resources(${ARGN})
        endmacro()

        macro(qt_add_translation)
          qt5_add_translation(${ARGN})
        endmacro()
    else (USE_QT5)
        find_package ( Qt4 COMPONENTS QtCore QtGui QtXml QtNetwork REQUIRED )
        set (JUFF_QT_IMPORTED_TARGETS Qt4::QtCore Qt4::QtGui Qt4::QtXml Qt4::QtNetwork)

        macro(qt_wrap_ui)
          qt4_wrap_ui(${ARGN})
        endmacro()

        macro(qt_add_resources)
          qt4_add_resources(${ARGN})
        endmacro()

        macro(qt_add_translation)
          qt4_add_translation(${ARGN})
        endmacro()
    endif (USE_QT5)

    find_package ( Juffed REQUIRED )
    
    set ( LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib )
    if (UNIX)
        set(JUFFED_PLUGINS_DIR ${LIB_INSTALL_DIR}/juffed/plugins)
    endif (UNIX)
 
endif (NOT JUFFED_CMAKE_PLUGINS)


set(CMAKE_INCLUDE_CURRENT_DIR 1)

include_directories(${JUFFED_INCLUDE_DIR})

if (BUILD_DOCLIST)
    add_subdirectory(doclist)
endif (BUILD_DOCLIST)

if (BUILD_FAVORITES)
    add_subdirectory(favorites)
endif (BUILD_FAVORITES)

if (BUILD_FM)
    add_subdirectory(fm)
endif (BUILD_FM)

if (BUILD_SYMBOLBROWSER)
    add_subdirectory(symbolbrowser)
endif (BUILD_SYMBOLBROWSER)

if (BUILD_FINDFILES)
    add_subdirectory(findinfiles)
endif (BUILD_FINDFILES)

if (BUILD_TERMINAL)
    add_subdirectory(terminal)
endif (BUILD_TERMINAL)

if (BUILD_XMLFORMAT)
    add_subdirectory(xmlformat)
endif (BUILD_XMLFORMAT)

if (BUILD_SORT)
    add_subdirectory(sort)
endif (BUILD_SORT)

if (BUILD_TODOLIST)
    add_subdirectory(todolist)
endif (BUILD_TODOLIST)

if (BUILD_KEYS)
    add_subdirectory(keybindings)
endif (BUILD_KEYS)

if (BUILD_AUTOSAVE)
    add_subdirectory(autosave)
endif (BUILD_AUTOSAVE)

if (BUILD_COLORPICKER)
    add_subdirectory(colorpicker)
endif (BUILD_COLORPICKER)

if (BUILD_COMPARE)
    add_subdirectory(compare)
endif (BUILD_COMPARE)
