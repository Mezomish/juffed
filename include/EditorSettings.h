/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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

#ifndef __JUFFED_EDITOR_SETTINGS_H__
#define __JUFFED_EDITOR_SETTINGS_H__

#include <QColor>
#include <QFont>

class EditorSettings {
public:
	enum IntKey {
		FontSize,
		TabWidth,
		LineLengthIndicator,
	};
	
	enum BoolKey {
		UseTabs,
		ShowLineNumbers,
		ShowWhitespaces,
		ShowLineEnds,
		WrapWords,
	};
	
	enum StringKey {
		FontFamily,
	};
	
	enum ColorKey {
		DefaultBgColor,
		DefaultFontColor,
		SelectionBgColor,
	};
	
	static void set(IntKey, int);
	static void set(BoolKey, bool);
	static void set(StringKey, const QString&);
	static void set(ColorKey, const QColor&);

	static int     get(IntKey);
	static bool    get(BoolKey);
	static QString get(StringKey);
	static QColor  get(ColorKey);
	
	static QFont font();
	static void setFont(const QFont&);
};

#endif // __JUFFED_EDITOR_SETTINGS_H__
