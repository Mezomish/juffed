#include "JuffAPI.h"

#include "PluginNotifier.h"

class JuffAPI::Interior {
public:
	Juff::DocHandlerInt* handler_;
	Juff::PluginNotifier* notifier_;
	CommandStorageInt* cmdStrg_;
	IconManagerInt* iconMgr_;
};

JuffAPI::JuffAPI(Juff::DocHandlerInt* handler, Juff::PluginNotifier* notifier, CommandStorageInt* cs, IconManagerInt* im) : QObject() {
	int_ = new Interior();
	int_->handler_ = handler;
	int_->notifier_ = notifier;
	int_->cmdStrg_ = cs;
	int_->iconMgr_ = im;
	
	connect(notifier, SIGNAL(docOpened(Juff::Document*, Juff::PanelIndex)), SIGNAL(docOpened(Juff::Document*, Juff::PanelIndex)));
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
	connect(notifier, SIGNAL(settingsApplied()), SIGNAL(settingsApplied()));
}

JuffAPI::~JuffAPI() {
	delete int_;
}

Juff::Document* JuffAPI::currentDocument() const {
	return int_->handler_->curDoc();
}

Juff::Document* JuffAPI::currentDocument(Juff::PanelIndex panel) const {
	return int_->handler_->curDoc(panel);
}

Juff::Document* JuffAPI::document(const QString& fileName) const {
	return int_->handler_->getDoc(fileName);
}

Juff::Project* JuffAPI::currentProject() const {
	return int_->handler_->curPrj();
}

CommandStorageInt* JuffAPI::commandStorage() const {
	return int_->cmdStrg_;
}

IconManagerInt* JuffAPI::iconManager() const {
	return int_->iconMgr_;
}

QWidget* JuffAPI::mainWindow() const {
	return int_->handler_->mainWindow();
}


void JuffAPI::openDoc(const QString& fileName, Juff::PanelIndex panel) {
	int_->handler_->openDoc(fileName, panel);
}

void JuffAPI::closeDoc(const QString& fileName) {
	int_->handler_->closeDoc(fileName);
}

void JuffAPI::saveDoc(const QString& fileName) {
	int_->handler_->saveDoc(fileName);
}

int JuffAPI::docCount(Juff::PanelIndex panel) const {
	return int_->handler_->docCount(panel);
}

QStringList JuffAPI::docList() const {
	return int_->handler_->docList();
}
