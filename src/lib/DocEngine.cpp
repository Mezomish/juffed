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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "DocEngine.h"

#include "Log.h"
#include "NullDoc.h"

#include <QAction>
#include <QMenu>

namespace Juff {

DocEngine::DocEngine() {
	curDoc_ = NullDoc::instance();
}

DocEngine::~DocEngine() {
}

Juff::Document* DocEngine::curDoc() const {
	return curDoc_;
}

QAction* DocEngine::addAction(Juff::MenuID id, QAction* action) {
	if ( !actionsMap_.contains(id) ) {
		actionsMap_.insert(id, QList<QAction*>());
	}
	actionsMap_[id] << action;
	
	return action;
}

void DocEngine::activate(bool activate) {
//	LOGGER;
	
	foreach (QList<QAction*> list, actionsMap_) {
		foreach (QAction* act, list) {
			act->setVisible(activate);
		}
	}
}

void DocEngine::deactivate(bool deact) {
//	LOGGER;
	
	foreach (QList<QAction*> list, actionsMap_) {
		foreach (QAction* act, list) {
			act->setVisible(!deact);
		}
	}
}

void DocEngine::onDocActivated(Juff::Document* doc) {
	curDoc_ = ( doc == NULL ? NullDoc::instance() : doc );
}

} // namespace Juff
