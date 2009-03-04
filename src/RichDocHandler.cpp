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
	tb->addAction("B");
	tb->addAction("I");
	tb->addAction("U");
	toolBars_.append(tb);
	
	QMenu* formatMenu = new QMenu("Format");
	formatMenu->addAction("Bold");
	formatMenu->addAction("Italic");
	formatMenu->addAction("Underline");
	menus_.append(formatMenu);
}

QString RichDocHandler::type() const {
	return "rich";
}

Document* RichDocHandler::createDoc(const QString& fileName) {
	Document* doc = new RichDoc(fileName);
	setDocType(doc, type());
	return doc;
}

ToolBarList RichDocHandler::toolBars() const {
	return toolBars_;
}

MenuList RichDocHandler::menus() const {
	return menus_;
}

}	//	namespace Juff
