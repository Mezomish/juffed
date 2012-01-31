# symbolbrowser
QT4_WRAP_CPP(symbolbrowser_MOC
    SymbolBrowser.h
#    symbols/parsers/parserex.h
    symbols/parsers/parser.h
#    symbols/parsers/vstring.h
#    symbols/parsers/keyword.h
    symbols/docsymbols.h
    symbols/symboltreeview.h
    symbols/parserthread.h
#    symbols/symbol.h
#    symbols/common.h
    )
set(symbolbrowser_SRC
    symbols/docsymbols.cpp
    symbols/parsers/keyword.cpp
    symbols/parsers/parserex.cpp
    symbols/parsers/vstring.cpp
    symbols/parsers/parser.cpp
    symbols/symboltreeview.cpp
    symbols/parserthread.cpp
    symbols/symbol.cpp
    SymbolBrowser.cpp
    )
QT4_ADD_RESOURCES(symbolbrowser_RES symbols/symbols.qrc)
add_library(symbolbrowser SHARED ${symbolbrowser_SRC} ${symbolbrowser_RES} ${symbolbrowser_MOC})
target_link_libraries(symbolbrowser ${JUFFED_LIBRARY} ${QT_LIBRARIES})
install(TARGETS symbolbrowser DESTINATION ${JUFFED_PLUGINS_DIR})
