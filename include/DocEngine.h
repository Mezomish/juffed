#ifndef __JUFFED_DOC_ENGINE_H__
#define __JUFFED_DOC_ENGINE_H__

namespace Juff {
	class Document;
	class DocHandlerInt;
}

class QAction;
class QMenu;

#include "Enums.h"

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QWidgetList>

class DocEngine {
public:
	static void setDocHandler(Juff::DocHandlerInt*);

	virtual Juff::Document* createDoc(const QString& fileName) const = 0;
	virtual QString type() const = 0;
	virtual QStringList syntaxList() const { return QStringList(); }

	virtual void initMenuActions(Juff::MenuID, QMenu*) {}
	virtual QWidgetList statusWidgets() { return QWidgetList(); }
	virtual void activate(bool act = true);
	virtual void deactivate(bool deact = true);

protected:
	void addAction(Juff::MenuID, QMenu*, QAction*);
	
	static Juff::Document* curDoc();

private:
	QMap< Juff::MenuID, QList<QAction*> > actionsMap_;

	static Juff::DocHandlerInt* handler_;
};

#endif // __JUFFED_DOC_ENGINE_H__
