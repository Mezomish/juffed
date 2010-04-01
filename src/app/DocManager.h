#ifndef __JUFF_DOC_MANAGER_H__
#define __JUFF_DOC_MANAGER_H__

class JuffMW;
class QMenu;
class QStatusBar;

#include "Enums.h"

#include <QMap>
#include <QString>

namespace Juff {
	class Document;
	class DocHandlerInt;
}
class DocEngine;

class DocManager {
public:
	DocManager(Juff::DocHandlerInt*);

	Juff::Document* newDoc(const QString& type = "");
	Juff::Document* openDoc(const QString& fileName, const QString& type = "");

	void initMenuActions(Juff::MenuID, QMenu*);
	void initStatusBar(QStatusBar*);
	void setCurDocType(const QString&);

private:
	void initEngines();

	QMap<QString, DocEngine*> engines_;
	Juff::DocHandlerInt* handler_;
};

#endif // __JUFF_DOC_MANAGER_H__
