# doclist
set(sort_SRC sort.cpp QuickSettings.cpp)
QT4_WRAP_UI(sort_UI QuickSettings.ui)
QT4_WRAP_CPP(sort_MOC sort.h QuickSettings.h)
add_library(sortdocument SHARED ${sort_SRC} ${sort_MOC} ${sort_UI})
target_link_libraries(sortdocument ${JUFFED_LIBRARY} ${QT_LIBRARIES})
install(TARGETS sortdocument DESTINATION ${JUFFED_PLUGINS_DIR})

