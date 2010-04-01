#include "DocManager.h"

#include "DocHandlerInt.h"
#include "JuffMW.h"
#include "Log.h"
#include "NullDoc.h"
#include "SciDocEngine.h"

#include <QStatusBar>

static const QString DefaultType = "QSci";

DocManager::DocManager(Juff::DocHandlerInt* handler) {
	LOGGER;
	
	handler_ = handler;
	initEngines();
}

void DocManager::initEngines() {
	LOGGER;
	
	DocEngine* eng = new SciDocEngine();
	eng->setDocHandler(handler_);
	engines_[eng->type()] = eng;
}

Juff::Document* DocManager::newDoc(const QString& type) {
	QString docType = type.isEmpty() ? DefaultType : type;
	Juff::Document* doc;
	if (engines_.contains(docType))
		doc = engines_[docType]->createDoc("");
	else
		doc = NullDoc::instance();
//	doc->init();
	return doc;
}

Juff::Document* DocManager::openDoc(const QString& fileName, const QString& type) {
	QString docType = type.isEmpty() ? DefaultType : type;
	if (engines_.contains(docType))
		return engines_[docType]->createDoc(fileName);
	else
		return NullDoc::instance();
}

void DocManager::initMenuActions(Juff::MenuID id, QMenu* menu) {
	LOGGER;
	
	foreach (DocEngine* eng, engines_) {
		eng->initMenuActions(id, menu);
		eng->deactivate();
	}
}

void DocManager::initStatusBar(QStatusBar* sb) {
	LOGGER;
	
	foreach ( DocEngine* eng, engines_ ) {
		QWidgetList widgets = eng->statusWidgets();
		foreach ( QWidget* w, widgets ) {
			sb->addWidget(w);
		}
	}
}

void DocManager::setCurDocType(const QString& type) {
	LOGGER;

	QMap<QString, DocEngine*>::iterator it = engines_.begin();
	for (; it != engines_.end(); it++) {
		if ( it.key() == type ) {
			it.value()->activate();
		}
		else {
			it.value()->deactivate();
		}
	}
}
