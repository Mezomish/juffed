# terminal
find_package(QTermWidget REQUIRED)

include_directories(${QTERMWIDGET_INCLUDE_DIR})

QT4_WRAP_CPP(terminal_MOC terminal/TerminalPlugin.h)
QT4_WRAP_UI(terminal_UI terminal/preferences.ui)
set(terminal_SRC terminal/TerminalPlugin.cpp)
add_library(terminal SHARED ${terminal_SRC} ${terminal_MOC} ${terminal_UI})
target_link_libraries(terminal ${JUFFED_LIBRARY} ${QT_LIBRARIES} ${QTERMWIDGET_LIBRARIES})
install(TARGETS terminal DESTINATION ${JUFFED_PLUGINS_DIR})

