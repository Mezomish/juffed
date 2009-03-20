CONFIG		+= release
TEMPLATE	= lib
CONFIG		+= plugin
DESTDIR		= ..
TARGET		= plugin
LIBS		+= -ljuff
INCLUDEPATH	+= ../../include
DEFINES		+= JUFF_DEBUG

HEADERS		= Plugin.h

SOURCES		= Plugin.cpp

