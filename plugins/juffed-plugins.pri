# Some common variables for all plugins.
# This file needs to be included 

unix {
	isEmpty(LIB_SUFFIX) {
#		HARDWARE_PLATFORM = $$system(uname -m)
#		contains( HARDWARE_PLATFORM, 64 ) {
		contains(QMAKE_HOST.arch, x86_64) {
			# 64-bit
			LIB_SUFFIX=64
		}
	}
}

isEmpty(PREFIX) {
	PREFIX = /usr
}

LIBS        -= -L$${PREFIX}/lib
LIBS        += -L$${PREFIX}/lib$${LIB_SUFFIX} -ljuff
INCLUDEPATH += $${PREFIX}/include/
INCLUDEPATH += $${PREFIX}/include/juffed

target.path  = $${PREFIX}/lib$${LIB_SUFFIX}/juffed/plugins
INSTALLS    += target

DEFINES     += JUFF_DEBUG


TEMPLATE     = lib
CONFIG      += plugin
DESTDIR      = ..
#CONFIG     += release
