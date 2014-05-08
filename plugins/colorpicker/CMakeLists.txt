# colorpicker
set( colorpicker_SRC
    ColorPickerPlugin.cpp
    ColorFormatDlg.cpp
)

QT4_ADD_RESOURCES( colorpicker_RES colorpicker.qrc )
QT4_WRAP_UI( colorpicker_UI ColorFormatDlg.ui)

add_library( colorpicker SHARED
    ${colorpicker_SRC}
    ${colorpicker_RES}
    ${colorpicker_UI}
)

target_link_libraries( colorpicker
    ${JUFFED_LIBRARY}
    ${QT_LIBRARIES}
)

install( TARGETS colorpicker DESTINATION ${JUFFED_PLUGINS_DIR} )
