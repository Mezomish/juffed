# doclist
set(sort_SRC sort.cpp QuickSettings.cpp)

QT4_WRAP_UI(sort_UI QuickSettings.ui)

add_library(sortdocument SHARED ${sort_SRC} ${sort_UI})
target_link_libraries(sortdocument ${JUFFED_LIBRARY} ${QT_LIBRARIES})
install(TARGETS sortdocument DESTINATION ${JUFFED_PLUGINS_DIR})
