TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
		doclist \
		favorites \
#		findinfiles \
		fm \
#		keybindings \
		symbolbrowser \

!win32 {
	SUBDIRS += terminal
}

