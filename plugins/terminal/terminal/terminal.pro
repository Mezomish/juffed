include(../../juffed-plugins.pri)

TARGET      = terminal
DESTDIR     = ../..

HEADERS     = TerminalPlugin.h
SOURCES     = TerminalPlugin.cpp
FORMS       = preferences.ui

INCLUDEPATH += ../qtermwidget/lib
LIBS        += -L../qtermwidget -lqtermwidget
