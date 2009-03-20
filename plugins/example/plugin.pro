CONFIG		+= release
TEMPLATE	= lib
CONFIG		+= plugin
DESTDIR		= ..
TARGET		= plugin
LIBS		+= -ljuff
DEFINES		+= JUFF_DEBUG

HEADERS		= Plugin.h

SOURCES		= Plugin.cpp

