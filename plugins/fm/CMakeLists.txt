# fm

QT4_ADD_RESOURCES(fm_RES fm.qrc)
QT4_WRAP_UI(fm_UI ManageDlg.ui)

set(fm_SRC
    ManageDlg.cpp
    FMPlugin.cpp
    TreeView.cpp
   )

add_library(fm SHARED ${fm_SRC} ${fm_RES} ${fm_UI})
target_link_libraries(fm ${JUFFED_LIBRARY} ${JUFF_QT_IMPORTED_TARGETS})
install(TARGETS fm DESTINATION ${JUFFED_PLUGINS_DIR})
