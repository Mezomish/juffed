/*
JuffEd - A simple text editor
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

#include "RichDocHandler.h"

#include <QtGui/QMenu>
#include <QtGui/QToolBar>

#include "RichDoc.h"

namespace Juff {

RichDocHandler::RichDocHandler() {
	QToolBar* tb = new QToolBar("RichEditToolBar");
	tb->addAction("B", this, SLOT(bold()));
	tb->addAction("I", this, SLOT(italic()));
	tb->addAction("U", this, SLOT(underline()));
	toolBars_.append(tb);

	boldAct_ = new QAction("Bold", this);
	italicAct_ = new QAction("Italic", this);
	ulAct_ = new QAction("Underline", this);

	connect(boldAct_, SIGNAL(activated()), SLOT(bold()));
	connect(italicAct_, SIGNAL(activated()), SLOT(italic()));
	connect(ulAct_, SIGNAL(activated()), SLOT(underline()));
}

RichDocHandler::~RichDocHandler() {
}

QString RichDocHandler::type() const {
	return "rich";
}

Document* RichDocHandler::createDoc(const QString& fileName) {
	Document* doc = new RichDoc(fileName);
	setDocType(doc, type());
	return doc;
}

ActionList RichDocHandler::menuActions(MenuID id) const {
	ActionList list;
	switch ( id ) {
		case ID_MENU_FORMAT :
			list << boldAct_ << italicAct_ << ulAct_;
			break;
		
		default: ;
	}
	return list;
}

ToolBarList RichDocHandler::toolBars() const {
	return toolBars_;
}

QString RichDocHandler::fileFilters() const {
	return "HTML files (*.html, *.htm);;All files (*)";
}


void RichDocHandler::bold() {
	JUFFENTRY;

	Juff::RichDoc* doc = qobject_cast<Juff::RichDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() ) {
		doc->bold();
	}
}

void RichDocHandler::italic() {
	JUFFENTRY;

	Juff::RichDoc* doc = qobject_cast<Juff::RichDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() ) {
		doc->italic();
	}
}

void RichDocHandler::underline() {
	JUFFENTRY;

	Juff::RichDoc* doc = qobject_cast<Juff::RichDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() ) {
		doc->underline();
	}
}

}	//	namespace Juff
