# keybindings
set(keys_SRC KeysPlugin.cpp KeysPage.cpp)

QT4_WRAP_UI(keys_UI KeysPage.ui)

add_library(keybindings SHARED ${keys_SRC} ${keys_UI})
target_link_libraries(keybindings ${JUFFED_LIBRARY} ${QT_LIBRARIES})
install(TARGETS keybindings DESTINATION ${JUFFED_PLUGINS_DIR})
