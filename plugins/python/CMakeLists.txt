# python

find_package ( PythonLibs REQUIRED)
INCLUDE_DIRECTORIES ( ${PYTHON_INCLUDE_PATH} )

set ( python_SRC PythonPlugin.cpp )
QT4_WRAP_CPP ( python_MOC PythonPlugin.h )
add_library ( python SHARED ${python_SRC} ${python_MOC} )
target_link_libraries ( python ${JUFFED_LIBRARY} ${QT_LIBRARIES} ${PYTHON_LIBRARIES} )
install ( TARGETS python DESTINATION ${JUFFED_PLUGINS_DIR} )

