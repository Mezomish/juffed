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

#ifndef _TEXT_DOC_SETTINGS_H_
#define _TEXT_DOC_SETTINGS_H_

#include "Settings.h"

#include <QtGui/QColor>
#include <QtGui/QFont>

class TextDocSettings : public Settings {
public:
	static QFont font();
	static int tabStopWidth();
	static bool widthAdjust();
	static bool showLineNumbers();
	static int lineLengthIndicator();
	static bool showIndents();
	static bool highlightCurrentLine();
	static bool replaceTabsWithSpaces();
	static bool backspaceUnindents();
	static QColor markersColor();
	static QColor curLineColor();
	
	static void setFont(const QFont& font) { Settings::setValue("editor", "fontFamily", font.family()); Settings::setValue("editor", "fontSize", font.pointSize()); }
	static void setWidthAdjust(bool adj) { Settings::setValue("editor", "widthAdjust", adj); }
	static void setShowLineNumbers(bool show) { Settings::setValue("editor", "showLineNumbers", show); }
	static void setTabStopWidth(int w) { Settings::setValue("editor", "tabStopWidth", w); }
	static void setLineLengthIndicator(int ind) { Settings::setValue("editor", "lineLengthIndicator", ind); }
	static void setShowIndents(bool show) { Settings::setValue("editor", "showIndents", show); }
	static void setHighlightCurrentLine(bool hl) { Settings::setValue("editor", "highlightCurrentLine", hl); }
	static void setReplaceTabsWithSpaces(bool replace) { Settings::setValue("editor", "replaceTabsWithSpaces", replace); }
	static void setBackspaceUnindents(bool unindents) { Settings::setValue("editor", "backspaceUnindents", unindents); }
	static void setMarkersColor(const QColor& color) { Settings::setValue("editor", "markersColor", color); }
	static void setCurLineColor(const QColor& color) { Settings::setValue("editor", "curLineColor", color); }
};

#endif
