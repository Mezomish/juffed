#include "Functions.h"

#include "Document.h"

#include <QFileInfo>

namespace Juff {

bool isNoname(const QString& fileName) {
	return fileName.left(6) == "Noname";
}

bool isNoname(Juff::Document* doc) {
	return isNoname(doc->fileName());
}

QString docTitle(const QString& fileName, bool modified) {
	if ( isNoname(fileName) ) {
		return QString(modified ? "*" : "") + QObject::tr("Noname %1").arg(fileName.section(' ', 1, 1).toInt());
	}
	else {
		return QString(modified ? "*" : "") + QFileInfo(fileName).fileName();
	}
}

QString docTitle(Juff::Document* doc) {
	return docTitle(doc->fileName(), doc->isModified());
}

}
