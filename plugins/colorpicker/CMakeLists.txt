# colorpicker
set( colorpicker_SRC
	ColorPickerPlugin.cpp
)

QT4_ADD_RESOURCES( colorpicker_RES colorpicker.qrc )
QT4_WRAP_CPP( colorpicker_MOC ColorPickerPlugin.h )

add_library( colorpicker SHARED
	${colorpicker_SRC}
	${colorpicker_MOC}
	${colorpicker_RES}
)

target_link_libraries( colorpicker
	${JUFFED_LIBRARY}
	${QT_LIBRARIES}
)

install( TARGETS colorpicker DESTINATION ${JUFFED_PLUGINS_DIR} )
