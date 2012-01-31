include(../juffed-plugins.pri)

TEMPLATE = subdirs
CONFIG  += ordered
TARGET   = terminal

SUBDIRS  = qtermwidget \
            terminal
