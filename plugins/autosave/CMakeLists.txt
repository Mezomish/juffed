# doclist
set(autosave_SRC autosave.cpp)
#QT4_WRAP_UI(autosave_UI)
QT4_WRAP_CPP(autosave_MOC autosave.h)
add_library(autosave SHARED ${autosave_SRC} ${autosave_MOC} ${autosave_UI})
target_link_libraries(autosave ${JUFFED_LIBRARY} ${QT_LIBRARIES})
install(TARGETS autosave DESTINATION ${JUFFED_PLUGINS_DIR})

