#include "JuffAPI.h"

class JuffAPI::Interior {
public:
	Juff::DocHandlerInt* handler_;
	Juff::PluginNotifier* notifier_;
};

JuffAPI::JuffAPI(Juff::DocHandlerInt* handler, Juff::PluginNotifier* notifier) : QObject() {
	int_ = new Interior();
	int_->handler_ = handler;
	int_->notifier_ = notifier;
	
	connect(notifier, SIGNAL(docOpened(Juff::Document*)), SIGNAL(docOpened(Juff::Document*)));
	connect(notifier, SIGNAL(docActivated(Juff::Document*)), SIGNAL(docActivated(Juff::Document*)));
	connect(notifier, SIGNAL(docClosed(Juff::Document*)), SIGNAL(docClosed(Juff::Document*)));
	connect(notifier, SIGNAL(docRenamed(Juff::Document*, const QString&)), SIGNAL(docRenamed(Juff::Document*, const QString&)));
	connect(notifier, SIGNAL(docModified(Juff::Document*)), SIGNAL(docModified(Juff::Document*)));
	connect(notifier, SIGNAL(docTextChanged(Juff::Document*)), SIGNAL(docTextChanged(Juff::Document*)));
	connect(notifier, SIGNAL(docSyntaxChanged(Juff::Document*, const QString&)), SIGNAL(docSyntaxChanged(Juff::Document*, const QString&)));
	connect(notifier, SIGNAL(docCharsetChanged(Juff::Document*, const QString&)), SIGNAL(docCharsetChanged(Juff::Document*, const QString&)));
	connect(notifier, SIGNAL(projectOpened(Juff::Project*)), SIGNAL(projectOpened(Juff::Project*)));
	connect(notifier, SIGNAL(projectRenamed(Juff::Project*, const QString&, const QString&)), SIGNAL(projectRenamed(Juff::Project*, const QString&, const QString&)));
	connect(notifier, SIGNAL(projectFileAdded(Juff::Project*, const QString&)), SIGNAL(projectFileAdded(Juff::Project*, const QString&)));
	connect(notifier, SIGNAL(projectFileRemoved(Juff::Project*, const QString&)), SIGNAL(projectFileRemoved(Juff::Project*, const QString&)));
	connect(notifier, SIGNAL(projectSubProjectAdded(Juff::Project*, Juff::Project*)), SIGNAL(projectSubProjectAdded(Juff::Project*, Juff::Project*)));
	connect(notifier, SIGNAL(projectSubProjectRemoved(Juff::Project*, Juff::Project*)), SIGNAL(projectSubProjectRemoved(Juff::Project*, Juff::Project*)));
	connect(notifier, SIGNAL(projectAboutToBeClosed(Juff::Project*)), SIGNAL(projectAboutToBeClosed(Juff::Project*)));
}

JuffAPI::~JuffAPI() {
	delete int_;
}

Juff::Document* JuffAPI::currentDocument() const {
	return int_->handler_->curDoc();
}

Juff::Document* JuffAPI::document(const QString& fileName) const {
	return int_->handler_->getDoc(fileName);
}

Juff::Project* JuffAPI::currentProject() const {
	return int_->handler_->curPrj();
}


void JuffAPI::openDoc(const QString& fileName) {
	int_->handler_->openDoc(fileName);
}

void JuffAPI::closeDoc(const QString& fileName) {
	int_->handler_->closeDoc(fileName);
}

void JuffAPI::saveDoc(const QString& fileName) {
	int_->handler_->saveDoc(fileName);
}

int JuffAPI::docCount() const {
	return int_->handler_->docCount();
}

QStringList JuffAPI::docList() const {
	return int_->handler_->docList();
}
