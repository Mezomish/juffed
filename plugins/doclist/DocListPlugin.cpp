#include "DocListPlugin.h"

#include "Document.h"
#include "DocListPanel.h"
#include "Log.h"


DocListPlugin::DocListPlugin()
    : QObject(),
      JuffPlugin(),
      panel_(0)
{
}

void DocListPlugin::init() {
    if (!panel_)
        panel_ = new DocListPanel(api());
	connect(api(), SIGNAL(docOpened(Juff::Document*, Juff::PanelIndex)), SLOT(onDocOpened(Juff::Document*, Juff::PanelIndex)));
	connect(api(), SIGNAL(docActivated(Juff::Document*)), SLOT(onDocActivated(Juff::Document*)));
    connect(api(), SIGNAL(docClosed(Juff::Document*)), SLOT(onDocClosed(Juff::Document*)));
	connect(api(), SIGNAL(docRenamed(Juff::Document*, const QString&)), SLOT(onDocRenamed(Juff::Document*, const QString&)));
	connect(api(), SIGNAL(docModified(Juff::Document*)), SLOT(onDocModified(Juff::Document*)));
}

DocListPlugin::~DocListPlugin() { 
    if (panel_)
        panel_->deleteLater();
}

QString DocListPlugin::name() const {
	return "Documents List";
}

QString DocListPlugin::title() const {
	return tr("Documents List");
}

QString DocListPlugin::targetEngine() const {
	return "all";
}

QString DocListPlugin::description() const {
	return "Plugin that allows to view all opened files as a list";
}

QWidgetList DocListPlugin::dockList() const {
	QWidgetList wList;
	wList << panel_;
	return wList;
}

bool DocListPlugin::dockVisible(QWidget* w) const {
	if ( w == panel_ ) {
		return true;
	}
	else {
		return JuffPlugin::dockVisible(w);
	}
}

void DocListPlugin::onDocOpened(Juff::Document* doc, Juff::PanelIndex) {
//	LOGGER;
    panel_->docOpened(doc->fileName(), doc->title());
}

void DocListPlugin::onDocActivated(Juff::Document* doc) {
//	LOGGER;
    panel_->docActivated(doc->fileName());
}

void DocListPlugin::onDocClosed(Juff::Document* doc) {
//	LOGGER;
    panel_->docClosed(doc->fileName());
}

void DocListPlugin::onDocRenamed(Juff::Document* doc, const QString& oldName) {
//	LOGGER;
    panel_->docRenamed(doc->fileName(), doc->title(), oldName);
}

void DocListPlugin::onDocModified(Juff::Document* doc) {
//	LOGGER;
    panel_->docModified(doc->fileName(), doc->titleWithModification());
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(doclist, DocListPlugin)
#endif
