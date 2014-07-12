# doclist
set (doclist_SRC
    DocListPlugin.cpp
    DocListPanel.cpp
)

QT_ADD_RESOURCES(doclist_RES doclistplugin.qrc)

add_library(doclist SHARED ${doclist_SRC} ${doclist_RES})
target_link_libraries(doclist ${JUFFED_LIBRARY} ${JUFF_QT_IMPORTED_TARGETS})
install(TARGETS doclist DESTINATION ${JUFFED_PLUGINS_DIR})
