# doclist
set(autosave_SRC autosave.cpp)

add_library(autosave SHARED ${autosave_SRC} ${autosave_UI})
target_link_libraries(autosave ${JUFFED_LIBRARY} ${JUFF_QT_IMPORTED_TARGETS})
install(TARGETS autosave DESTINATION ${JUFFED_PLUGINS_DIR})
