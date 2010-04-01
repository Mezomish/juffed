#ifndef __JUFFED_EDITOR_SETTINGS_H__
#define __JUFFED_EDITOR_SETTINGS_H__

#include "Settings.h"

#include <QColor>
#include <QFont>

class EditorSettings : public Settings {
public:
	enum IntKey {
		FontSize,
		TabWidth,
	};
	
	enum BoolKey {
		UseTabs,
		ShowLineNumbers,
		ShowWhitespaces,
	};
	
	enum StringKey {
		FontFamily,
	};
	
	enum ColorKey {
		DefaultBgColor,
		DefaultFontColor,
		CurLineColor,
		SelectionBgColor,
	};
	
	static void set(IntKey, int);
	static void set(BoolKey, bool);
	static void set(StringKey, const QString&);

	static int     get(IntKey);
	static bool    get(BoolKey);
	static QString get(StringKey);
	static QColor  get(ColorKey);
	
	static QFont font();
};

#endif // __JUFFED_EDITOR_SETTINGS_H__
