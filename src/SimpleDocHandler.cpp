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

#include "SimpleDocHandler.h"

#include <QtGui/QMenu>
#include <QtGui/QToolBar>

#include "SimpleDoc.h"

namespace Juff {

SimpleDocHandler::SimpleDocHandler() : DocHandler() {
	QToolBar* tb = new QToolBar("SimpleDocToolBar");
	tb->addAction("aaa");
	tb->addAction("bbb");
	toolBars_.append(tb);
	
	QMenu* editMenu = new QMenu("Edit");
	editMenu->addAction("Cut");
	editMenu->addAction("Copy");
	editMenu->addAction("Paste");
	menus_.append(editMenu);
}

QString SimpleDocHandler::type() const {
	return "simple";
}

Document* SimpleDocHandler::createDoc(const QString& fileName) {
	Document* doc = new SimpleDoc(fileName);
	setDocType(doc, type());
	return doc;
}

ToolBarList SimpleDocHandler::toolBars() const {
	return toolBars_;
}

MenuList SimpleDocHandler::menus() const {
	return menus_;
}

}	//	namespace Juff
