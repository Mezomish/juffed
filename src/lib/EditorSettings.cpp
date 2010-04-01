#include "EditorSettings.h"

void EditorSettings::set(IntKey key, int value) {
	switch (key) {
		case FontSize :
			Settings::setValue("editor", "fontSize", value);
			break;
		
		case TabWidth :
			Settings::setValue("editor", "tabStopWidth", value); 
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
	}
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
}

QString EditorSettings::get(StringKey key) {
	switch (key) {
		case FontFamily :
			return stringValue("editor", "fontFamily");
	}
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
}


QFont EditorSettings::font() {
	return QFont(get(FontFamily), get(FontSize));
}
