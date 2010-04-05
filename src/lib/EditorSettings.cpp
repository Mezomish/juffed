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

#include "EditorSettings.h"

void EditorSettings::set(IntKey key, int value) {
	switch (key) {
		case FontSize :
			Settings::setValue("editor", "fontSize", value);
			break;
		
		case TabWidth :
			Settings::setValue("editor", "tabStopWidth", value); 
			break;
		
		case LineLengthIndicator :
			Settings::setValue("editor", "lineLengthIndicator", value); 
			break;
	}
}

void EditorSettings::set(BoolKey key, bool value) {
	switch (key) {
		case UseTabs :
			Settings::setValue("editor", "replaceTabsWithSpaces", !value); 
			break;
		case ShowLineNumbers :
			Settings::setValue("editor", "showLineNumbers", value); 
			break;
		case ShowWhitespaces:
			Settings::setValue("editor", "showWhitespaces", value); 
			break;
	}
}

void EditorSettings::set(StringKey key, const QString& value) {
	switch (key) {
		case FontFamily :
			Settings::setValue("editor", "fontFamily", value);
			break;
	}
}



int EditorSettings::get(IntKey key) {
	switch (key) {
		case FontSize :
			return Settings::intValue("editor", "fontSize");
		case TabWidth :
			return Settings::intValue("editor", "tabStopWidth");
		case LineLengthIndicator :
			return Settings::intValue("editor", "lineLengthIndicator");
	}
	return -1;
}

bool EditorSettings::get(BoolKey key) {
	switch (key) {
		case UseTabs :
			return !Settings::boolValue("editor", "replaceTabsWithSpaces");
		case ShowLineNumbers :
			return Settings::boolValue("editor", "showLineNumbers");
		case ShowWhitespaces:
			return Settings::boolValue("editor", "showWhitespaces");
	}
	return false;
}

QString EditorSettings::get(StringKey key) {
	switch (key) {
		case FontFamily :
			return stringValue("editor", "fontFamily");
	}
	return "";
}

QColor EditorSettings::get(ColorKey key) {
	switch (key) {
		case CurLineColor :
		{
			QColor deflt(240, 240, 255);
			QColor c = Settings::value("editor", "curLineColor").value<QColor>();
			return c.isValid() ? c : deflt;
		}
		case SelectionBgColor :
		{
			QColor deflt(150, 150, 155);
			QColor c = Settings::value("editor", "selectionBgColor").value<QColor>();
			return c.isValid() ? c : deflt;
		}
		case DefaultBgColor :
		{
			QColor deflt(255, 255, 255);
			QColor c = Settings::value("editor", "defaultBgColor").value<QColor>();
			return c.isValid() ? c : deflt;
		}
		case DefaultFontColor :
		{
			QColor deflt(0, 0, 0);
			QColor c = Settings::value("editor", "defaultFontColor").value<QColor>();
			return c.isValid() ? c : deflt;
		}
	}
	return QColor();
}

void EditorSettings::set(ColorKey key, const QColor& c) {
	switch (key) {
		case CurLineColor :
			Settings::setValue("editor", "curLineColor", c);
			break;
		
		case SelectionBgColor :
			Settings::setValue("editor", "selectionBgColor", c);
			break;
		
		case DefaultBgColor :
			Settings::setValue("editor", "defaultBgColor", c);
			break;
		
		case DefaultFontColor :
			Settings::setValue("editor", "defaultFontColor", c);
			break;
	}
}

QFont EditorSettings::font() {
	return QFont(get(FontFamily), get(FontSize));
}

void EditorSettings::setFont(const QFont& f) {
	set(FontFamily, f.family());
	set(FontSize, f.pointSize());
}
