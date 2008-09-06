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
bool TextDocSettings::backspaceUnindents() {
	return Settings::boolValue("editor", "backspaceUnindents", false);
}
QColor TextDocSettings::markersColor() {
	QColor deflt(100, 200, 100);
	QColor c = Settings::value("editor", "markersColor").value<QColor>();
	return c.isValid() ? c : deflt;
}
QColor TextDocSettings::curLineColor() {
	QColor deflt(230, 250, 230);
	QColor c = Settings::value("editor", "curLineColor").value<QColor>();
	return c.isValid() ? c : deflt;
}


void TextDocSettings::setFont(const QFont& font) { 
	Settings::setValue("editor", "fontFamily", font.family()); Settings::setValue("editor", "fontSize", font.pointSize()); 
}
void TextDocSettings::setWidthAdjust(bool adj) { 
	Settings::setValue("editor", "widthAdjust", adj); 
}
void TextDocSettings::setShowLineNumbers(bool show) { 
	Settings::setValue("editor", "showLineNumbers", show); 
}
void TextDocSettings::setTabStopWidth(int w) { 
	Settings::setValue("editor", "tabStopWidth", w); 
}
void TextDocSettings::setLineLengthIndicator(int ind) { 
	Settings::setValue("editor", "lineLengthIndicator", ind); 
}
void TextDocSettings::setShowIndents(bool show) { 
	Settings::setValue("editor", "showIndents", show); 
}
void TextDocSettings::setHighlightCurrentLine(bool hl) { 
	Settings::setValue("editor", "highlightCurrentLine", hl); 
}
void TextDocSettings::setReplaceTabsWithSpaces(bool replace) { 
	Settings::setValue("editor", "replaceTabsWithSpaces", replace); 
}
void TextDocSettings::setBackspaceUnindents(bool unindents) { 
	Settings::setValue("editor", "backspaceUnindents", unindents); 
}
void TextDocSettings::setMarkersColor(const QColor& color) { 
	Settings::setValue("editor", "markersColor", color); 
}
void TextDocSettings::setCurLineColor(const QColor& color) { 
	Settings::setValue("editor", "curLineColor", color); 
}
