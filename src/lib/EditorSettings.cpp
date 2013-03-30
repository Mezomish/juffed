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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "EditorSettings.h"
#include "Settings.h"

#include <QPalette>

void EditorSettings::set(IntKey key, int value) {
	switch (key) {
		case FontSize :
			Settings::instance()->setValue("editor", "fontSize", value);
			break;

		case TabWidth :
			Settings::instance()->setValue("editor", "tabStopWidth", value);
			break;

		case LineLengthIndicator :
			Settings::instance()->setValue("editor", "lineLengthIndicator", value);
			break;
	}
}

void EditorSettings::set(BoolKey key, bool value) {
	switch (key) {
		case UseTabs :
			Settings::instance()->setValue("editor", "replaceTabsWithSpaces", !value);
			break;

		case BackspaceUnindents :
			Settings::instance()->setValue("editor", "backspaceUnindents", value);
			break;

		case ShowLineNumbers :
			Settings::instance()->setValue("editor", "showLineNumbers", value);
			break;

		case ShowWhitespaces :
			Settings::instance()->setValue("editor", "showWhitespaces", value);
			break;

		case ShowLineEnds :
			Settings::instance()->setValue("editor", "showLineEnds", value);
			break;

		case WrapWords :
			Settings::instance()->setValue("editor", "wrapWords", value);
			break;
	}
}

void EditorSettings::set(StringKey key, const QString& value) {
	switch (key) {
		case FontFamily :
			Settings::instance()->setValue("editor", "fontFamily", value);
			break;
	}
}



int EditorSettings::get(IntKey key) {
	switch (key) {
		case FontSize :
			return Settings::instance()->intValue("editor", "fontSize");

		case TabWidth :
			return Settings::instance()->intValue("editor", "tabStopWidth");

		case LineLengthIndicator :
			return Settings::instance()->intValue("editor", "lineLengthIndicator");
	}
	return -1;
}

bool EditorSettings::get(BoolKey key) {
	switch (key) {
		case UseTabs :
			return !Settings::instance()->boolValue("editor", "replaceTabsWithSpaces");

		case BackspaceUnindents :
			return Settings::instance()->boolValue("editor", "backspaceUnindents");

		case ShowLineNumbers :
			return Settings::instance()->boolValue("editor", "showLineNumbers");

		case ShowWhitespaces :
			return Settings::instance()->boolValue("editor", "showWhitespaces");

		case ShowLineEnds :
			return Settings::instance()->boolValue("editor", "showLineEnds");

		case WrapWords :
			return Settings::instance()->boolValue("editor", "wrapWords");
	}
	return false;
}

QString EditorSettings::get(StringKey key) {
	switch (key) {
		case FontFamily :
			return Settings::instance()->stringValue("editor", "fontFamily");
	}
	return "";
}

QColor EditorSettings::get(ColorKey key) {
	switch (key) {
		case SelectionBgColor :
		{
			QColor deflt = QPalette().color(QPalette::Highlight);
			QColor c = Settings::instance()->value("editor", "selectionBgColor").value<QColor>();
			return c.isValid() ? c : deflt;
		}
		case DefaultBgColor :
		{
			QColor deflt = QPalette().color(QPalette::Base);
			QColor c = Settings::instance()->value("editor", "defaultBgColor").value<QColor>();
			return c.isValid() ? c : deflt;
		}
		case DefaultFontColor :
		{
			QColor deflt = QPalette().color(QPalette::Text);
			QColor c = Settings::instance()->value("editor", "defaultFontColor").value<QColor>();
			return c.isValid() ? c : deflt;
		}
	}
	return QColor();
}

void EditorSettings::set(ColorKey key, const QColor& c) {
	switch (key) {
		case SelectionBgColor :
			Settings::instance()->setValue("editor", "selectionBgColor", c);
			break;
		
		case DefaultBgColor :
			Settings::instance()->setValue("editor", "defaultBgColor", c);
			break;
		
		case DefaultFontColor :
			Settings::instance()->setValue("editor", "defaultFontColor", c);
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
