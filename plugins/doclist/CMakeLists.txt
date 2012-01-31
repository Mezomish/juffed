# doclist
set (doclist_SRC
	DocListPlugin.cpp
	DocListPanel.cpp
)

QT4_ADD_RESOURCES(doclist_RES doclistplugin.qrc)

QT4_WRAP_CPP( doclist_MOC
	DocListPlugin.h
	DocListPanel.h
)

#QT4_AUTOMOC(DocListPlugin.h DocListPanel.h)
add_library(doclist SHARED ${doclist_SRC} ${doclist_RES} ${doclist_MOC})
target_link_libraries(doclist ${JUFFED_LIBRARY} ${QT_LIBRARIES})
install(TARGETS doclist DESTINATION ${JUFFED_PLUGINS_DIR})

