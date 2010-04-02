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

#include "DocEngine.h"

#include "DocHandlerInt.h"
#include "Log.h"
#include "NullDoc.h"

#include <QAction>
#include <QMenu>

Juff::DocHandlerInt* DocEngine::handler_ = NULL;

void DocEngine::setDocHandler(Juff::DocHandlerInt* h) {
	handler_ = h;
}

Juff::Document* DocEngine::curDoc() {
	if ( handler_ != NULL )
		return handler_->curDoc();
	else
		return NullDoc::instance();
}


void DocEngine::addAction(Juff::MenuID id, QMenu* menu, QAction* action) {
	if ( !actionsMap_.contains(id) ) {
		actionsMap_.insert(id, QList<QAction*>());
	}
	actionsMap_[id] << action;
	menu->addAction(action);
}

void DocEngine::activate(bool act) {
	LOGGER;
	
	foreach (QList<QAction*> list, actionsMap_) {
		foreach (QAction* act, list) {
			act->setVisible(act);
		}
	}
}

void DocEngine::deactivate(bool deact) {
	LOGGER;
	
	foreach (QList<QAction*> list, actionsMap_) {
		foreach (QAction* act, list) {
			act->setVisible(!deact);
		}
	}
}
