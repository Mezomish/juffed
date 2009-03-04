#ifndef _JUFF_FUNCTIONS_H_
#define _JUFF_FUNCTIONS_H_

#include <QtCore/QFileInfo>
#include <QtCore/QString>

namespace Juff {

inline bool isNoname(const QString& fileName) {
	return fileName.left(6) == "Noname";
}

inline QString getDocTitle(const QString& fileName, bool modified = false) {
	if ( isNoname(fileName) ) {
		return QObject::tr("Noname") + (modified ? "*" : "");
	}
	else {
		return QFileInfo(fileName).fileName() + (modified ? "*" : "");
	}
}

}	//	namespace Juff

#endif
