CONFIG		+= release
TEMPLATE	= lib
CONFIG		+= plugin
DESTDIR		= ..
TARGET		= plugin
LIBS		+= -ljuff
INCLUDEPATH	+= ../../include

HEADERS		= Plugin.h

SOURCES		= Plugin.cpp

