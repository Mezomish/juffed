# juffed CMakeLists.txt
# 
# To build juffed, you will need cmake 2.8.11.
# To build the application run this commands:
# 
# mkdir build
# cd build
# cmake .. -DCMAKE_INSTALL_PREFIX=/usr (or whatever prefix you'd like)
# make
# make install
#
# mac os x notes:
# juffed.app is build by default. There are some additions to steps above. 
# "make install" is required to get plugins working
# "make bundle" an optional make target to build bundle with all dependencies inside
#


project (juffed)
cmake_minimum_required (VERSION 3.7)

# build options
option(USE_QT6 "Use Qt6. Defaults to Qt4" ON)
option(USE_QT5 "Use Qt5. Defaults to Qt4" OFF)
option(USE_SYSTEM_QTSINGLEAPPLICATION "Try to find system-wide QtSingleApplication library" $ENV{USE_SYSTEM_SINGLETON})
option(USE_ENCA "Try to find Enca library for file encoding check" $ENV{USE_ENCA})


if (COMMAND cmake_policy)
    CMAKE_POLICY (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )
set ( PREFIX ${CMAKE_INSTALL_PREFIX} )

# switch verbose mode ON or OFF
set ( CMAKE_VERBOSE_MAKEFILE $ENV{VERBOSE_MAKEFILE} )

####################
# Names and Version
####################

set ( JUFFED  "juffed" CACHE STRING "Name of the executeable" )
set ( JUFFLIB "juff"   CACHE STRING "Name of the library" )
set ( QSCI_ENGINE "juffed-engine-qsci" CACHE STRING "" )
file   ( READ "${CMAKE_SOURCE_DIR}/version"      JUFFED_VERSION )
file   ( READ "${CMAKE_SOURCE_DIR}/base_version" BASE_VERSION )
string ( STRIP "${JUFFED_VERSION}" JUFFED_VERSION )
string ( STRIP "${BASE_VERSION}"   BASE_VERSION )

# flag indicating that this is a development version
set ( DEV 0 )

if ( DEV )
    # SVN revision
#    find_package(Subversion)
#    if(Subversion_FOUND)
#        Subversion_WC_INFO(${PROJECT_SOURCE_DIR} SVN)
#    endif(Subversion_FOUND)
#    set ( REV ${SVN_WC_REVISION} )
#    set ( JUFFED_VERSION "${BASE_VERSION}.${REV}" )
endif ( DEV )

message ( STATUS "" )
message ( STATUS "${JUFFED} version: ${JUFFED_VERSION}" )


####################
# Directories
####################

include ( cmake/LibSuffix.cmake )

set (BIN_INSTALL_DIR
    "${PREFIX}/bin"
    CACHE PATH "Install path for executables"
)

set (LIB_INSTALL_DIR
    "${PREFIX}/lib${LIB_SUFFIX}"
    CACHE PATH "Install path for libraries"
)

set (DATA_INSTALL_DIR
    "${PREFIX}/share/${CMAKE_PROJECT_NAME}"
    CACHE PATH "Install path for shared data"
)

message ( STATUS "Install path for executables: ${BIN_INSTALL_DIR}" )
message ( STATUS "Install path for libraries: ${LIB_INSTALL_DIR}" )
message ( STATUS "" )
#set (CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}")

set (CMAKE_AUTOMOC ON)

if (USE_QT6)
    find_package ( Qt6 COMPONENTS Core5Compat Widgets Xml Network PrintSupport LinguistTools CONFIG REQUIRED )
    set (JUFF_QT_IMPORTED_TARGETS Qt6::Core5Compat Qt6::Widgets Qt6::Xml Qt6::Network Qt6::PrintSupport)

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

# this is mandatory for Apple builds to load plugin correctly
add_definitions(-DQT_NO_DEBUG)

find_package ( QScintilla2 REQUIRED)


include ( cmake/FileSets.cmake )
include ( cmake/SomeLexers.cmake )

if(USE_SYSTEM_QTSINGLEAPPLICATION)
    find_package ( QtSingleApplication REQUIRED)
else(USE_SYSTEM_QTSINGLEAPPLICATION)
    set(QTSINGLEAPPLICATION_INCLUDE_DIR src/3rd_party/qtsingleapplication)
    SET ( juffed_app_SRCS
        ${juffed_app_SRCS}
        src/3rd_party/qtsingleapplication/qtsinglecoreapplication.cpp
        src/3rd_party/qtsingleapplication/qtsingleapplication.cpp
        src/3rd_party/qtsingleapplication/qtlockedfile.cpp
        src/3rd_party/qtsingleapplication/qtlocalpeer.cpp
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
endif(USE_SYSTEM_QTSINGLEAPPLICATION)


QT_ADD_RESOURCES ( juffed_app_RESOURCES ${juffed_app_RESOURCES} )
QT_ADD_RESOURCES ( juffed_lib_RESOURCES ${juffed_lib_RESOURCES} )
QT_WRAP_UI ( juffed_qsci_UIS_H ${juffed_qsci_UIS} )
QT_WRAP_UI ( juffed_app_UIS_H ${juffed_app_UIS} )
QT_ADD_TRANSLATION ( juffed_QM ${juffed_TS} )


include(FindPkgConfig)
if (USE_ENCA)
# try to find libenca - optional
pkg_check_modules(ENCA enca)
if (ENCA_FOUND)
    add_definitions(-DHAVE_ENCA)
    include_directories(${ENCA_INCLUDE_DIRS})
    link_directories(${ENCA_LIBRARY_DIRS})
    message(STATUS "Enca includes: ${ENCA_INCLUDE_DIRS}")
    message(STATUS "Enca library dir: ${ENCA_LIBRARY_DIRS}")
    message(STATUS "Enca library: ${ENCA_LIBRARIES}")
else (ENCA_FOUND)
    message(WARNING "No enca library found. Building without auto-language-detection")
endif (ENCA_FOUND)
endif (USE_ENCA)


# include directories
include_directories(
    ${CMAKE_BINARY_DIR}
    ${QSCINTILLA_INCLUDE_DIR}
    ${QSCINTILLA_INCLUDE_DIR}/Qsci
    ${QTSINGLEAPPLICATION_INCLUDE_DIR}
    src/app/ui
    src/app/qsci
    ./include
)

configure_file ("${CMAKE_SOURCE_DIR}/cmake/AppInfo.nix.h.cmake" "${CMAKE_BINARY_DIR}/AppInfo.nix.h" @ONLY)
configure_file ("${CMAKE_SOURCE_DIR}/cmake/AppInfo.win.h.cmake" "${CMAKE_BINARY_DIR}/AppInfo.win.h" @ONLY)
configure_file ("${CMAKE_SOURCE_DIR}/cmake/AppInfo.apple.h.cmake" "${CMAKE_BINARY_DIR}/AppInfo.apple.h" @ONLY)


####################
# Targets
####################

# library
add_library ( ${JUFFLIB} SHARED
    ${juffed_lib_SRCS}
    ${juffed_lib_MOC_HEADERS}
    ${juffed_lib_RESOURCES}
)
target_link_libraries ( ${JUFFLIB}
    ${JUFF_QT_IMPORTED_TARGETS}
)
if (ENCA_FOUND)
    target_link_libraries( ${JUFFLIB} ${ENCA_LIBRARIES} )
endif (ENCA_FOUND)

add_library ( ${QSCI_ENGINE} SHARED
    ${juffed_qsci_SRCS}
    ${juffed_qsci_UIS_H}
)
target_link_libraries ( ${QSCI_ENGINE}
    ${JUFFLIB}
    ${JUFF_QT_IMPORTED_TARGETS}
    ${QSCINTILLA_LIBRARY} 
    ${QTSINGLEAPPLICATION_LIBRARY} 
)

if (APPLE)
    message ( STATUS "Building as an apple bundle" )
    set ( APPLE_PREFIX "${CMAKE_INSTALL_PREFIX}/${JUFFED}.app/Contents" )
    
    add_definitions(-DAPPLEBUNDLE)
    set(GUI_TYPE MACOSX_BUNDLE)
    # create Info.plist file
    set(MACOSX_BUNDLE_ICON_FILE juffed.icns)
    set(MACOSX_BUNDLE_INFO_STRING "juffed ${JUFFED_VERSION}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.juffed")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${JUFFED_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${JUFFED}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${JUFFED_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${JUFFED_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "(c) juffed.com")
    set_source_files_properties(${CMAKE_SOURCE_DIR}/macosx/juffed.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    # use icon for app bundle to be visible in finder
    set(APPLE_BUNDLE_SOURCES ${CMAKE_SOURCE_DIR}/macosx/juffed.icns)
    # this is a must to load the lib correctly
    set_target_properties( ${JUFFLIB} PROPERTIES INSTALL_NAME_DIR "@executable_path/../MacOS" )
    set_target_properties( ${QSCI_ENGINE} PROPERTIES INSTALL_NAME_DIR "@executable_path/../MacOS" )
else (APPLE)
    set(GUI_TYPE "")
    set(APPLE_BUNDLE_SOURCES "")
    # now we want to use soversion etc.
    if (NOT APPLE)
        # this causes a build fail on mac (unix-like build)
        set_target_properties ( ${JUFFLIB} PROPERTIES VERSION ${BASE_VERSION} SOVERSION ${BASE_VERSION} )
        set_target_properties ( ${QSCI_ENGINE} PROPERTIES VERSION ${BASE_VERSION} SOVERSION ${BASE_VERSION} )
    else (NOT APPLE)
        # apple requires this one to keep sane dyn-linking
        set_target_properties( ${JUFFLIB} PROPERTIES INSTALL_NAME_DIR ${LIB_INSTALL_DIR} )
        set_target_properties( ${QSCI_ENGINE} PROPERTIES INSTALL_NAME_DIR ${LIB_INSTALL_DIR} )
    endif (NOT APPLE)
endif (APPLE)

if ( MINGW )
    add_definitions(
        -DQSCINTILLA_DLL
    )

    add_definitions( -D__SPECIAL_WINDOWS_DEFINE__ )

    # resource compilation for MinGW
    add_custom_command ( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/app/juffed_ico.o
        COMMAND $ENV{WINDRES} -I${CMAKE_CURRENT_SOURCE_DIR}/src/app -i${CMAKE_CURRENT_SOURCE_DIR}/src/app/juffed.rc 
            -o ${CMAKE_CURRENT_BINARY_DIR}/src/app/juffed_ico.o )
    set ( juffed_app_SRCS ${juffed_app_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/src/app/juffed_ico.o)
endif ( MINGW )


# executable
add_executable ( ${JUFFED}
    ${GUI_TYPE}
    ${juffed_app_SRCS} 
    ${juffed_UIS} 
    ${juffed_app_UIS_H}
    ${juffed_app_RESOURCES} 
    ${juffed_QM} 
    ${APPLE_BUNDLE_SOURCES}
)

if ( MINGW )
    set_target_properties( ${JUFFED} PROPERTIES LINK_FLAGS "-mwindows" )
endif ( MINGW )

target_link_libraries ( ${JUFFED}
    ${JUFFLIB}
    ${QSCI_ENGINE}
    ${JUFF_QT_IMPORTED_TARGETS}
)

if ( UNIX )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif ( UNIX )

#
# plugins
message(STATUS "Plugins: all externally linked cmake-ready plugs will be built")
set(JUFFED_CMAKE_PLUGINS 1)
if (UNIX)
    set(JUFFED_PLUGINS_DIR ${LIB_INSTALL_DIR}/juffed/plugins)
endif (UNIX)
if (APPLE)
    set(JUFFED_PLUGINS_DIR ${APPLE_PREFIX}/plugins)
endif (APPLE)
if (WIN32)
    set(JUFFED_PLUGINS_DIR ${CMAKE_INSTALL_PREFIX}/plugins)
endif(WIN32)
set(JUFFED_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(JUFFED_LIBRARY ${JUFFLIB})
add_subdirectory(plugins)

#configure_file ("${CMAKE_SOURCE_DIR}/cmake/version.cmake" "${CMAKE_BINARY_DIR}/version.real" @ONLY)




####################
# Installs
####################
if (NOT APPLE)

    install ( TARGETS ${JUFFED} DESTINATION ${BIN_INSTALL_DIR} )
    install ( TARGETS ${JUFFLIB} DESTINATION ${LIB_INSTALL_DIR} )
    install ( TARGETS ${QSCI_ENGINE} DESTINATION ${LIB_INSTALL_DIR} )
    install ( FILES ${juffed_QM} DESTINATION ${DATA_INSTALL_DIR}/l10n )
    install ( DIRECTORY hlschemes DESTINATION ${DATA_INSTALL_DIR} PATTERN ".svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
    install ( DIRECTORY apis DESTINATION ${DATA_INSTALL_DIR} PATTERN "*.svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
    install ( FILES juffed.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )
    install ( FILES juffed.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
    install ( FILES ${juffed_DEV_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${CMAKE_PROJECT_NAME} )

    ## uninstall 
    configure_file ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )
    add_custom_target ( uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

else (NOT APPLE)

    install(CODE "message(STATUS \"Cleaning previously installed bundle (rm -r)\")")
    install(CODE "execute_process(COMMAND rm -r ${CMAKE_INSTALL_PREFIX}/${JUFFED}.app)")

    install( TARGETS ${JUFFED} DESTINATION "${CMAKE_INSTALL_PREFIX}" )
    # lib is linked directly into binary in bundles to prevent Info.plist hacking
    install( TARGETS ${JUFFLIB} DESTINATION "${APPLE_PREFIX}/MacOS" )
    install( TARGETS ${QSCI_ENGINE} DESTINATION "${APPLE_PREFIX}/MacOS" )
    install ( FILES ${juffed_QM} DESTINATION "${APPLE_PREFIX}/Resources/l10n" )

    install ( DIRECTORY hlschemes DESTINATION "${APPLE_PREFIX}/Resources" PATTERN ".svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
    install ( DIRECTORY apis DESTINATION "${APPLE_PREFIX}/Resources" PATTERN "*.svn" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )

    # helper stuff to create real apple bundle.
    # Black magic is summoned here...
        # create a "transportable" bundle - all libs into the bundle: "make bundle" after make install
        configure_file(macosx/bundle.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake @ONLY)
        add_custom_target(bundle ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake)

endif (NOT APPLE)


# make dist custom target
SET(CPACK_PACKAGE_NAME ${JUFFED})
SET(CPACK_PACKAGE_VERSION ${JUFFED_VERSION})
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP;")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;/\\\\.git/;\\\\.swp$;\\\\.#;/#;\\\\.tar.gz$;\\\\~$;/CMakeFiles/;CMakeCache.txt;\\\\.qm$;/win32/;/build/;")
SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})
INCLUDE(CPack)
# simulate autotools' "make dist"
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
