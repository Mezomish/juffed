# doclist
set(compare_SRC compare.cpp)

QT_ADD_RESOURCES(compare_RES compare.qrc)

add_library(compare SHARED ${compare_SRC} ${compare_RES})
target_link_libraries(compare ${JUFFED_LIBRARY} ${JUFF_QT_IMPORTED_TARGETS})
install(TARGETS compare DESTINATION ${JUFFED_PLUGINS_DIR})
