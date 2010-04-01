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
