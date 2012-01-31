# todolist
QT4_WRAP_CPP ( todolist_MOC TODOList.h TODOParser.h )
set ( todolist_SRC TODOList.cpp TODOParser.cpp )

add_library( todolist SHARED ${todolist_SRC} ${todolist_RES} ${todolist_MOC} )
target_link_libraries(todolist ${JUFFED_LIBRARY} ${QT_LIBRARIES})
install(TARGETS todolist DESTINATION ${JUFFED_PLUGINS_DIR})
