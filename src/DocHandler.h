/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

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

#ifndef _JUFF_DOC_HANDLER_H_
#define _JUFF_DOC_HANDLER_H_

#include <QtCore/QObject>
#include <QtGui/QWidgetList>

#include "Juff.h"
#include "Document.h"

namespace Juff {

class DocHandler : public QObject {
Q_OBJECT
public:
	DocHandler();
	virtual ~DocHandler();

	virtual QString type() const = 0;
	virtual QString fileFilters() const = 0;
	virtual Document* createDoc(const QString&) = 0;

	virtual MenuList menus() const;
	virtual ActionList menuActions(MenuID) const;
	virtual ToolBarList toolBars() const;
	virtual QWidgetList statusWidgets() const;

	virtual void addContextMenuActions(const ActionList&) {}

	virtual void docActivated(Document*) = 0;

signals:
	Juff::Document* getCurDoc();

protected:
	void setDocType(Document*, const QString& type);

private:
	class Interior;
	Interior* hInt_;
};

}	//	namespace Juff

#endif
