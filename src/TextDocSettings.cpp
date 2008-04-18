/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "TextDocSettings.h"

QFont TextDocSettings::font() { 
#ifdef Q_OS_WIN32
	QString fontFamily = Settings::stringValue("editor", "fontFamily", "Courier New");
#else
	QString fontFamily = Settings::stringValue("editor", "fontFamily", "DejaVu Sans Mono");
#endif
	int fontSize = Settings::intValue("editor", "fontSize", 10);
	return QFont(fontFamily, fontSize); 
}

int TextDocSettings::tabStopWidth() { 
	return Settings::intValue("editor", "tabStopWidth", 4); 
}

bool TextDocSettings::widthAdjust() { 
	return Settings::boolValue("editor", "widthAdjust", false);
}

bool TextDocSettings::showLineNumbers() { 
	return Settings::boolValue("editor", "showLineNumbers", true); 
}
	
int TextDocSettings::lineLengthIndicator() { 
	return Settings::intValue("editor", "lineLengthIndicator", 80); 
}

bool TextDocSettings::showIndents() {
	return Settings::boolValue("editor", "showIndents", true);
}

bool TextDocSettings::highlightCurrentLine() {
	return Settings::boolValue("editor", "highlightCurrentLine", true);
}

bool TextDocSettings::replaceTabsWithSpaces() {
	return Settings::boolValue("editor", "replaceTabsWithSpaces", false);
}
