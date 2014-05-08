# symbolbrowser

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

add_library(symbolbrowser SHARED ${symbolbrowser_SRC} ${symbolbrowser_RES})
target_link_libraries(symbolbrowser ${JUFFED_LIBRARY} ${JUFF_QT_IMPORTED_TARGETS})
install(TARGETS symbolbrowser DESTINATION ${JUFFED_PLUGINS_DIR})
