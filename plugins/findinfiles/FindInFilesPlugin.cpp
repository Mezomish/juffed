#include <QDebug>

#include "FindInFilesPlugin.h"

#include "SearchDlg.h"
#include "FindWorker.h"

#include <QtCore/QFileInfo>
#include <QAction>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QToolBar>
#include <QVBoxLayout>

//#include <CommandStorageInt.h>
#include <Log.h>
#include <PluginSettings.h>

class FindInFilesPlugin::PluginInterior {
public:
	PluginInterior(FindInFilesPlugin * parent) {
		ed_ = new QLineEdit();
		ed_->setMaximumWidth(200);
		searchAct_ = new QAction(tr("Search in files"), 0);
		searchAct_->setShortcut(QKeySequence("Ctrl+Alt+F"));
		stopAct_ = new QAction(QIcon(":stop"), tr("Stop searching"), 0);
		stopAct_->setEnabled(false);
		stopAct_->setVisible(false);
		searchAct_->setIcon(parent->api()->iconManager()->icon("main:find"));

		//	create toolbar
		toolBar_ = new QToolBar("Find In Files");
		toolBar_->setObjectName("FindInFilesToolBar");
		toolBar_->addWidget(new QLabel(tr("Find: ")));
		toolBar_->addWidget(ed_);
		toolBar_->addAction(searchAct_);
		toolBar_->addAction(stopAct_);
		
		//	create widget that will be embedded into dock 
		//	and put controls to it
		widget_ = new QWidget();
		widget_->setWindowTitle("Search results");
		tree_ = new QTreeWidget();
		QVBoxLayout* vBox = new QVBoxLayout();

		vBox->setMargin(0);
		vBox->addWidget(tree_);
		widget_->setLayout(vBox);

		QStringList labels;
		labels << "File" << "Line" << "Text" << "Column";
		tree_->setHeaderLabels(labels);
		tree_->setRootIsDecorated(false);
#if QT_VERSION < 0x050000
		tree_->header()->setResizeMode(QHeaderView::Interactive);
#else
		tree_->header()->setSectionResizeMode(QHeaderView::Interactive);
#endif
		tree_->header()->setAutoScroll(true);
		tree_->header()->setStretchLastSection(false);
		tree_->setColumnWidth(0, 500);
		tree_->setColumnWidth(1, 50);
		tree_->setColumnWidth(2, 500);
		tree_->setColumnHidden(3, true);
		tree_->setAlternatingRowColors(true);
	}
	~PluginInterior() {
		delete widget_;
	}
	
	QLineEdit* ed_;
	QAction* searchAct_;
	QAction* stopAct_;
	QToolBar* toolBar_;
	QWidget* widget_;
	QTreeWidget* tree_;
	FindWorker worker_;
};

FindInFilesPlugin::FindInFilesPlugin() : QObject(), JuffPlugin() {
	pInt_ = 0;
}

void FindInFilesPlugin::init() {
	pInt_ = new PluginInterior(this);
	
	connect(pInt_->ed_, SIGNAL(returnPressed()), SLOT(slotSearchInFiles()));
	connect(pInt_->tree_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(slotItemDoubleClicked(QTreeWidgetItem*, int)));
	connect(pInt_->searchAct_, SIGNAL(triggered()), SLOT(slotAdvSearch()));
	connect(pInt_->stopAct_, SIGNAL(triggered()), SLOT(slotStopSearch()));
	connect(&(pInt_->worker_), SIGNAL(matchFound(const QString, int, int, const QString&)),
	        SLOT(slotMatchFound(const QString, int, int, const QString&)));
	connect(&(pInt_->worker_), SIGNAL(started()), SLOT(onStarted()));
	connect(&(pInt_->worker_), SIGNAL(finished()), SLOT(onFinished()));
	connect(&(pInt_->worker_), SIGNAL(terminated()), SLOT(onFinished()));
}

FindInFilesPlugin::~FindInFilesPlugin() { 
	delete pInt_;
}

QString FindInFilesPlugin::name() const {
	return "Find In Files";
}

QString FindInFilesPlugin::targetEngine() const {
	return "all";
}

QString FindInFilesPlugin::description() const {
	return "";
}

QToolBar* FindInFilesPlugin::toolBar() const {
	return pInt_->toolBar_;
}


QWidgetList FindInFilesPlugin::dockList() const {
	QWidgetList list;
	list << pInt_->widget_;
	return list;
}

Qt::DockWidgetArea FindInFilesPlugin::dockPosition(QWidget* w) const {
	if ( w == pInt_->widget_ ) {
		return Qt::BottomDockWidgetArea;
	}
	else {
		return JuffPlugin::dockPosition(w);
	}
}

Juff::ActionList FindInFilesPlugin::mainMenuActions(Juff::MenuID id) const {
	Juff::ActionList list;
	if ( Juff::MenuTools == id ) {
		list << pInt_->searchAct_;
	}
	return list;
}


void FindInFilesPlugin::startSearch() {
	if ( PluginSettings::getBool(this, "searchInFiles") ) {
		slotSearchInFiles();
	}
	else {
		QString findText = pInt_->ed_->text();
		if ( findText.isEmpty() )
			return;

		showDock();
		QString startDir = PluginSettings::getString(this, "searchStartDir");
		bool rec = PluginSettings::getBool(this, "searchRecursively");
		int patternVariant = PluginSettings::getInt(this, "patternVariant", 0);
		QString filePatterns = PluginSettings::getString(this, "filePatterns", "");
		if ( pInt_->worker_.isRunning() ) {
			pInt_->worker_.terminate();
			pInt_->worker_.wait();
		}

		pInt_->worker_.setParams(FindWorker::Params(findText, startDir, rec, patternVariant, filePatterns.split(";")));
		pInt_->worker_.start(QThread::LowestPriority);
	}
}

void FindInFilesPlugin::findInFiles(const QString& findText, const QStringList& list) {
	foreach (QString fileName, list) {
		QString text;
		api()->document(fileName)->getText(text);
		findInText(findText, text, fileName);
	}
}

void FindInFilesPlugin::findInText(const QString& findText, const QString& text, const QString& fileName) {
	QStringList lines = text.split(QRegExp("\r\n|\n|\r"));
	int lineIndex = 0;
	foreach (QString line, lines) {
		int column = line.indexOf(findText);
		if ( column >= 0 ) {
			slotMatchFound(fileName, lineIndex, column, line);
		}
		++lineIndex;
	}
}


void FindInFilesPlugin::showDock() {
	pInt_->tree_->clear();
	QWidget* dock = pInt_->widget_->parentWidget();
	if ( dock != 0 && dock->isHidden() ) {
		dock->show();
	}
}

void FindInFilesPlugin::slotSearchInFiles() {
	QString findText = pInt_->ed_->text();
	if ( findText.isEmpty() )
		return;

	showDock();
	QStringList docs = api()->docList();
	findInFiles(findText, docs);
}

void FindInFilesPlugin::slotItemDoubleClicked(QTreeWidgetItem* item, int) {
	if (item == 0)
		return;

	api()->openDoc(item->text(0));
	api()->document(item->text(0))->setCursorPos(item->text(1).toInt() - 1, item->text(3).toInt());
}

void FindInFilesPlugin::slotAdvSearch() {
	QString text = pInt_->ed_->text();
	bool searchInFiles = PluginSettings::getBool(this, "searchInFiles");
	QString startDir = PluginSettings::getString(this, "searchStartDir");
	bool recursive = PluginSettings::getBool(this, "searchRecursively");
	QString filePatterns = PluginSettings::getString(this, "filePatterns");
	int patternVariant = PluginSettings::getInt(this, "patternVariant", 0);
	
	if ( startDir.isEmpty() ) {
		QString curFile = api()->currentDocument()->fileName();
		if ( !curFile.isEmpty() )
			startDir = QFileInfo(curFile).absolutePath();
	}
	
	SearchDlg dlg(api()->mainWindow());
	dlg.setFindText(text);
	dlg.setSearchInFiles(searchInFiles);
	dlg.setStartDir(startDir);
	dlg.setRecursive(recursive);
	dlg.setFilePatterns(filePatterns);
	dlg.setPatternVariant(patternVariant);
	
	int result = dlg.exec();
	PluginSettings::set(this, "searchInFiles", dlg.searchInFiles());
	PluginSettings::set(this, "searchStartDir", dlg.startDir());
	PluginSettings::set(this, "searchRecursively", dlg.recursive());
	PluginSettings::set(this, "filePatterns", dlg.filePatterns());
	PluginSettings::set(this, "patternVariant", dlg.patternVariant());

	if ( QDialog::Accepted == result ) {
		text = dlg.findText();
		pInt_->ed_->setText(text);
		PluginSettings::set(this, "searchText", text);
		startSearch();
	}
	
}

void FindInFilesPlugin::slotMatchFound(const QString fileName, int row, int col, const QString& line) {
	QStringList itemTexts;
	itemTexts << fileName << QString::number(row + 1) << line.trimmed() << QString::number(col);
	QTreeWidgetItem* item = new QTreeWidgetItem(itemTexts);
	pInt_->tree_->addTopLevelItem(item);
}


void FindInFilesPlugin::onStarted() {
	pInt_->searchAct_->setEnabled(false);
	pInt_->stopAct_->setEnabled(true);
	pInt_->searchAct_->setVisible(false);
	pInt_->stopAct_->setVisible(true);
}

void FindInFilesPlugin::onFinished() {
	pInt_->searchAct_->setEnabled(true);
	pInt_->stopAct_->setEnabled(false);
	pInt_->searchAct_->setVisible(true);
	pInt_->stopAct_->setVisible(false);
}

void FindInFilesPlugin::slotStopSearch() {
	pInt_->worker_.quit();
	if ( !pInt_->worker_.wait(2000) )
		pInt_->worker_.terminate();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(findinfiles, FindInFilesPlugin)
#endif
