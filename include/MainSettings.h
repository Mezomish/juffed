#ifndef __JUFFED_MAIN_SETTINGS_H__
#define __JUFFED_MAIN_SETTINGS_H__

#include "Settings.h"

class MainSettings :  public Settings {
public:
	enum StringKey {
		LastProject,
		LastDir,
	};
	
	enum BoolKey {
		SyncToCurDoc,
		Maximized,
		FullScreen,
	};
//	static void set(IntKey, int);
	static void set(BoolKey, bool);
	static void set(StringKey, const QString&);

//	static int     get(IntKey);
	static bool    get(BoolKey);
	static QString get(StringKey);
//	static QColor  get(ColorKey);
};

#endif // __JUFFED_MAIN_SETTINGS_H__
