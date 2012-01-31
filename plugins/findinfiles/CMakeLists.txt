# findinfiles
QT4_WRAP_CPP(findinfiles_MOC
    FindInFilesPlugin.h
    FindWorker.h
    SearchDlg.h
    )
QT4_WRAP_UI(findinfiles_UI SearchDlg.ui)
set(findinfiles_SRC
    FindInFilesPlugin.cpp
    FindWorker.cpp
    SearchDlg.cpp
    )
QT4_ADD_RESOURCES(findinfiles_RES findinfiles.qrc)
add_library(findinfiles SHARED ${findinfiles_SRC} ${findinfiles_RES} ${findinfiles_MOC} ${findinfiles_UI})
target_link_libraries(findinfiles ${JUFFED_LIBRARY} ${QT_LIBRARIES})
install(TARGETS findinfiles DESTINATION ${JUFFED_PLUGINS_DIR})


