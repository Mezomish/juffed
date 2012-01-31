#include <QDebug>

#include "TODOList.h"
#include "TODOParser.h"
#include "Log.h"

#include <QtCore>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

class TODOListPlugin::PluginInterior {
public:
	PluginInterior(TODOListPlugin*) {
		widget_ = new QWidget();
		widget_->setWindowTitle("TODO List");
		tree_ = new QTreeWidget();
		QVBoxLayout* vBox = new QVBoxLayout();

		vBox->setMargin(0);
		vBox->addWidget(tree_);
		widget_->setLayout(vBox);

		QStringList labels;
		labels << "Line" << "Line Number" << "Column";
		tree_->setHeaderLabels(labels);
		tree_->setRootIsDecorated(false);
		tree_->header()->setResizeMode(0, QHeaderView::Stretch);
		tree_->header()->setResizeMode(1, QHeaderView::Fixed);
		tree_->header()->setAutoScroll(true);
		tree_->header()->setStretchLastSection(false);
		tree_->header()->hide();
		tree_->setColumnWidth(1, 100);
		tree_->setColumnHidden(2, true);
		
		parser_ = NULL;
	}
	~PluginInterior() {
		delete widget_;
	}
	
	QWidget* widget_;
	QTreeWidget* tree_;
	
	TODOParser* parser_;
};

TODOListPlugin::TODOListPlugin() : QObject(), JuffPlugin() {
}

void TODOListPlugin::init() {
	pInt_ = new PluginInterior(this);
	
	connect(api(), SIGNAL(docActivated(Juff::Document*)), this, SLOT(onDocActivated(Juff::Document*)));
	connect(pInt_->tree_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
}

TODOListPlugin::~TODOListPlugin() { 
	delete pInt_;
}

QString TODOListPlugin::name() const {
	return "TODO List";
}

QString TODOListPlugin::targetEngine() const {
	return "all";
}

QString TODOListPlugin::description() const {
	return "Displays all TODOs, FIXMEs, etc in the current file";
}


QWidgetList TODOListPlugin::dockList() const {
	QWidgetList list;
	list << pInt_->widget_;
	return list;
}

Qt::DockWidgetArea TODOListPlugin::dockPosition(QWidget* w) const {
	if ( w == pInt_->widget_ ) {
		return Qt::BottomDockWidgetArea;
	}
	else {
		return JuffPlugin::dockPosition(w);
	}
}



void TODOListPlugin::showDock() {
	QWidget* dock = pInt_->widget_->parentWidget();
	if ( dock != 0 && dock->isHidden() ) {
		dock->show();
	}
}

void TODOListPlugin::hideDock() {
	QWidget* dock = pInt_->widget_->parentWidget();
	if ( dock != 0 && dock->isVisible() ) {
		dock->hide();
	}
}


void TODOListPlugin::addItem(const QString& lineStr, int line, int col, const QColor& color) {
	QStringList texts;
	texts << lineStr.trimmed() << QString::number(line + 1) << QString::number(col);
	QTreeWidgetItem* item = new QTreeWidgetItem(texts);
	item->setBackground(0, color);
	item->setBackground(1, color);
	pInt_->tree_->addTopLevelItem(item);
//	if ( ! pInt_->tree_->isVisible() ) {
//		showDock();
//	}
}

void TODOListPlugin::onParserFinished() {
//	LOGGER;
	TODOParser* parser = qobject_cast<TODOParser*>(sender());
	delete parser;
	parser = NULL;
}

void TODOListPlugin::onDocActivated(Juff::Document* doc) {
	if ( doc->isNull() ) {
		hideDock();
		return;
	}

	if ( pInt_->parser_ != NULL ) {
		pInt_->parser_->cancel();
	}

	pInt_->parser_ = new TODOParser();
	connect(pInt_->parser_, SIGNAL(finished()), this, SLOT(onParserFinished()));
	connect(pInt_->parser_, SIGNAL(itemFound(const QString&, int, int, const QColor&)), this, SLOT(addItem(const QString&, int, int, const QColor&)));

	pInt_->tree_->clear();
	hideDock();

	QString text;
	doc->getText(text);
	pInt_->parser_->launch(text);
}

void TODOListPlugin::onDocClosed(Juff::Document*) {
}

void TODOListPlugin::onItemDoubleClicked(QTreeWidgetItem* item, int) {
	if ( item == 0 ) {
		return;
	}

	api()->currentDocument()->setCursorPos(item->text(1).toInt() - 1, item->text(2).toInt());
}

Q_EXPORT_PLUGIN2(todolist, TODOListPlugin)
