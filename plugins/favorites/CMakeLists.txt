# favorites
set(favorites_SRC
    ManageDlg.cpp
    FavoritesPlugin.cpp
   )

QT4_WRAP_UI(favorites_UI ManageDlg.ui)

add_library(favorites SHARED ${favorites_SRC} ${favorites_UI})
target_link_libraries(favorites ${JUFFED_LIBRARY} ${JUFF_QT_IMPORTED_TARGETS})
install(TARGETS favorites DESTINATION ${JUFFED_PLUGINS_DIR})

