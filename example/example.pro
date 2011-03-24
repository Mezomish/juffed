TEMPLATE      = app
INCLUDEPATH  += . ../src/app/qsci ../include
LIBS         += -ljuffed-engine-qsci -ljuff
SOURCES      += example.cpp

win32 {
	CONFIG += release
	CONFIG -= debug
	LIBS += -L.
}
