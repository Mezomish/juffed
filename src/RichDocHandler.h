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

#ifndef _RICH_DOC_HANDLER_H_
#define _RICH_DOC_HANDLER_H_

#include "DocHandler.h"

namespace Juff {

class Document;

class RichDocHandler : public DocHandler {
Q_OBJECT
public:
	RichDocHandler();
	virtual ~RichDocHandler();

	virtual QString type() const;
	virtual QString fileFilters() const;
	virtual Document* createDoc(const QString&);

	virtual ActionList menuActions(MenuID) const;
	virtual ToolBarList toolBars() const;

	virtual void docActivated(Document*) {}

protected slots:
	void bold();
	void italic();
	void underline();

private:
	ToolBarList toolBars_;
	QAction* boldAct_;
	QAction* italicAct_;
	QAction* ulAct_;
};

}

#endif
