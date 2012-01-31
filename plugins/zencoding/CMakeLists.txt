# ZenCoding
set ( zen_SRC ZenCoding.cpp DocWrapper.cpp )
QT4_WRAP_CPP ( zen_MOC ZenCoding.h DocWrapper.h )
#QT4_AUTOMOC ( zen_MOC_SRC ZenCoding.cpp )
add_library ( zencoding SHARED ${zen_SRC} ${zen_MOC} ${zen_MOC_SRC} )
target_link_libraries ( zencoding ${JUFFED_LIBRARY} ${QT_LIBRARIES} QtScript )

install ( TARGETS zencoding DESTINATION ${JUFFED_PLUGINS_DIR} )
