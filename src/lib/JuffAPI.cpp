#include "JuffAPI.h"

class JuffAPI::Interior {
public:
	Juff::DocHandlerInt* handler_;
	Juff::PluginNotifier* notifier_;
};

JuffAPI::JuffAPI(Juff::DocHandlerInt* handler, Juff::PluginNotifier* notifier) {
	int_ = new Interior();
	int_->handler_ = handler;
	int_->notifier_ = notifier;
}

JuffAPI::~JuffAPI() {
	delete int_;
}

Juff::Document* JuffAPI::currentDocument() const {
	return int_->handler_->curDoc();
}

Juff::Project* JuffAPI::currentProject() const {
	return int_->handler_->curPrj();
}

Juff::PluginNotifier* JuffAPI::notifier() const {
	return int_->notifier_;
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
