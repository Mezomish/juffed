/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

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
	static bool showInvisibleSymbols();
	static int lineLengthIndicator();
	static bool showIndents();
	static bool highlightCurrentLine();
	static bool replaceTabsWithSpaces();
	static bool backspaceUnindents();
	static QColor markersColor();
	static QColor curLineColor();
	static QColor defaultFontColor();
	static QColor defaultBgColor();
	static QColor matchedBraceBgColor();
	static QColor indentsColor();

	static void setFont(const QFont& font);
	static void setWidthAdjust(bool adj);
	static void setShowLineNumbers(bool show);
	static void setShowInvisibleSymbols(bool show);
	static void setTabStopWidth(int w);
	static void setLineLengthIndicator(int ind);
	static void setShowIndents(bool show);
	static void setHighlightCurrentLine(bool hl);
	static void setReplaceTabsWithSpaces(bool replace);
	static void setBackspaceUnindents(bool unindents);
	static void setMarkersColor(const QColor& color);
	static void setCurLineColor(const QColor& color);
	static void setDefaultFontColor(const QColor&);
	static void setDefaultBgColor(const QColor&);
	static void setMatchedBraceBgColor(const QColor&);
	static void setIndentsColor(const QColor&);
};

#endif
