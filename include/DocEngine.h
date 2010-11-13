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

#ifndef __JUFFED_DOC_ENGINE_H__
#define __JUFFED_DOC_ENGINE_H__

#include "LibConfig.h"

class QAction;
class QMenu;

#include "Enums.h"

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QWidgetList>

namespace Juff {

class Document;
class DocHandlerInt;

class LIBJUFF_EXPORT DocEngine {
public:
	DocEngine();
	virtual ~DocEngine();

	static void setDocHandler(Juff::DocHandlerInt*);

	virtual Juff::Document* createDoc(const QString& fileName) const = 0;
	virtual QString type() const = 0;
	virtual QStringList syntaxList() const { return QStringList(); }

	virtual void initMenuActions(Juff::MenuID, QMenu*) {}
	virtual QWidgetList statusWidgets() { return QWidgetList(); }
	virtual void activate(bool act = true);
	virtual void deactivate(bool deact = true);

	virtual QWidget* settingsPage() const { return 0; }
	
protected:
	void addAction(Juff::MenuID, QMenu*, QAction*);
	
	static Juff::Document* curDoc();

private:
	QMap< Juff::MenuID, QList<QAction*> > actionsMap_;

	static Juff::DocHandlerInt* handler_;
};

} // namespace Juff

#endif // __JUFFED_DOC_ENGINE_H__
