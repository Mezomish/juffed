# terminal
set (terminal_SRC TerminalPlugin.cpp)
set (terminal_UI preferences.ui)

if (USE_QT5)
    find_package ( QTermWidget5 REQUIRED )
    qt5_wrap_ui(terminal_UI_SRC ${terminal_UI})
    set(qtermwidget_lib "qtermwidget5")
else (USE_QT5)
    find_package ( QTermWidget4 REQUIRED )
    qt4_wrap_ui(terminal_UI_SRC ${terminal_UI})
    set(qtermwidget_lib "qtermwidget4")
endif (USE_QT5)

add_library(terminal SHARED ${terminal_SRC} ${terminal_MOC} ${terminal_UI_SRC})
target_link_libraries(terminal ${JUFFED_LIBRARY} ${qtermwidget_lib})
install(TARGETS terminal DESTINATION ${JUFFED_PLUGINS_DIR})
