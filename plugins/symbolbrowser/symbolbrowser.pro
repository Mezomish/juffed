include(../juffed-plugins.pri)

TARGET = symbolbrowser

QMAKE_CXXFLAGS = -g -O0

HEADERS = SymbolBrowser.h \
    symbols/symbol.h \
    symbols/docsymbols.h \
    symbols/common.h \
    symbols/parserthread.h \
    symbols/parsers/vstring.h \
    symbols/parsers/parser.h \
    symbols/parsers/parserex.h \
    symbols/parsers/keyword.h \
    symbols/symboltreeview.h

SOURCES = SymbolBrowser.cpp \
    symbols/symbol.cpp \
    symbols/docsymbols.cpp \
    symbols/parsers/parser_cpp.cpp \
    symbols/parsers/parser_python.cpp \
    symbols/parsers/parser_perl.cpp \
    symbols/parserthread.cpp \
    symbols/parsers/vstring.cpp \
    symbols/parsers/parser.cpp \
    symbols/parsers/parserex.cpp \
    symbols/parsers/keyword.cpp \
    symbols/symboltreeview.cpp

RESOURCES += symbols/symbols.qrc
