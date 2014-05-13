# findinfiles

set(findinfiles_SRC
    FindInFilesPlugin.cpp
    FindWorker.cpp
    SearchDlg.cpp
    )

QT_WRAP_UI(findinfiles_UI SearchDlg.ui)
QT_ADD_RESOURCES(findinfiles_RES findinfiles.qrc)

add_library(findinfiles SHARED ${findinfiles_SRC} ${findinfiles_RES} ${findinfiles_UI})
target_link_libraries(findinfiles ${JUFFED_LIBRARY} ${JUFF_QT_IMPORTED_TARGETS})
install(TARGETS findinfiles DESTINATION ${JUFFED_PLUGINS_DIR})
