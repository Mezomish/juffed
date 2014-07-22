#include "FMPlugin.h"

#include <QCompleter>
#include <QHeaderView>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>

#include <Log.h>
#include <MainSettings.h>
#include <PluginSettings.h>
#include <EditorSettings.h>

#include "ManageDlg.h"
#include "TreeView.h"

FMPlugin::FMPlugin() : QObject(), JuffPlugin() {
	showAsTree = PluginSettings::getBool(this, "ShowAsTree", false);
    showHidden = PluginSettings::getBool(this, "ShowHidden", false);
    sortColumn = PluginSettings::getInt(this, "sortColumn", 0);

	w_ = new QWidget();
	w_->setWindowTitle(tr("Files"));

	QToolBar * toolBar = new QToolBar("File Browser Tools", w_);

    model_ = new QFileSystemModel;
    model_->setRootPath("");

	tree_ = new TreeView(this);
	tree_->setModel(model_);
	tree_->setDragDropMode(QAbstractItemView::DragOnly);
	tree_->setAllColumnsShowFocus(true);
    tree_->sortByColumn(sortColumn, Qt::AscendingOrder);
    tree_->setSortingEnabled(true);
	tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	tree_->header()->resizeSection(0, 180);
	tree_->header()->resizeSection(1, 80);
	tree_->header()->resizeSection(2, 60);
	tree_->header()->resizeSection(3, 180);
	tree_->initMenu();
	
	pathEd_ = new QLineEdit("");
	backBtn_ = toolBar->addAction(QIcon::fromTheme("go-previous", QIcon(":icon_back")), tr("Go Back"), this, SLOT(back()));
	toolBar->addAction(QIcon::fromTheme("go-up", QIcon(":icon_up")), tr("Go Up"), this, SLOT(up()));
	toolBar->addAction(QIcon::fromTheme("go-home", QIcon(":icon_home")), tr("Go to Home Directory"), this, SLOT(home()));
	toolBar->addAction(QIcon::fromTheme("folder-txt", QIcon(":icon_current")), tr("Go to current file's directory"), this, SLOT(curFileDir()));
	toolBar->addAction(QIcon::fromTheme("user-bookmarks", QIcon(":icon_bookmarks")), tr("Favorite Locations"), this, SLOT(favorites()));
	toolBar->addAction(QIcon::fromTheme("folder-new", QIcon(":icon_new_dir")), tr("New Directory"), this, SLOT(newDir()));

	pathEd_->setCompleter(new QCompleter(model_));

	QVBoxLayout* vBox = new QVBoxLayout();

	vBox->setMargin(0);
	vBox->setSpacing(2);

	vBox->addWidget(toolBar);
	vBox->addWidget(pathEd_);
	vBox->addWidget(tree_);

	w_->setLayout(vBox);

	connect(tree_, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(itemDoubleClicked(const QModelIndex&)));
	connect(tree_, SIGNAL(goUp()), SLOT(up()));
	connect(pathEd_, SIGNAL(returnPressed()), SLOT(textEntered()));
	connect(&fsWatcher_, SIGNAL(directoryChanged(const QString&)), SLOT(onDirChanged(const QString&)));
	
	QString favStr = PluginSettings::getString(this, "favorites");
	if ( !favStr.isEmpty() )
		favorites_ = favStr.split(";");
	favoritesMenu_ = new QMenu(w_);
	addToFavoritesAct_ = new QAction(tr("Add current path"), this);
	manageFavoritesAct_ = new QAction(tr("Manage favorites"), this);
	connect(addToFavoritesAct_, SIGNAL(triggered()), this, SLOT(addToFavorites()));
	connect(manageFavoritesAct_, SIGNAL(triggered()), this, SLOT(manageFavorites()));
	initFavoritesMenu();

	QString lastDir = PluginSettings::getString(this, "lastDir");
	if ( lastDir.isEmpty() )
		lastDir = QDir::homePath();
	cd(lastDir, false);
	backBtn_->setEnabled(false);
	
	applySettings();
}

FMPlugin::~FMPlugin() {
    sortColumn = tree_->header()->sortIndicatorSection();
    PluginSettings::set(this, "sortColumn", sortColumn);

	delete w_;
}

QString FMPlugin::name() const {
	return "File manager";
}

QString FMPlugin::title() const {
	return tr("File manager");
}

QString FMPlugin::targetEngine() const {
	return "all";
}

QString FMPlugin::description() const {
	return "Simple file manager that lets to open files without a dialog";
}

QWidgetList FMPlugin::dockList() const {
	QWidgetList wList;
	wList << w_;
	return wList;
}

bool FMPlugin::dockVisible(QWidget* w) const {
	if ( w == w_ ) {
		return true;
	}
	else {
		return JuffPlugin::dockVisible(w);
	}
}

void FMPlugin::initFavoritesMenu() {
	favoritesMenu_->clear();

	favoritesMenu_->addAction(addToFavoritesAct_);
	favoritesMenu_->addAction(manageFavoritesAct_);
	if ( !favorites_.isEmpty() )
		favoritesMenu_->addSeparator();
	foreach (QString item, favorites_) {
		favoritesMenu_->addAction(item, this, SLOT(goToFavorite()));
	}
}


void FMPlugin::cd(const QString& path, bool addToHistory /*= true*/) {
	if ( QFileInfo(path).isDir() ) {
		// remove currently watching paths
		QStringList dirs = fsWatcher_.directories();
		if ( !dirs.isEmpty() ) {
			fsWatcher_.removePaths(dirs);
		}
		
		if ( addToHistory ) {
			QString curPath = model_->filePath(tree_->rootIndex());
			history_.push(curPath);
			if ( !backBtn_->isEnabled() )
				backBtn_->setEnabled(true);
		}
		
		tree_->setRootIndex(model_->index(path));
		pathEd_->setText(path);
		pathEd_->setToolTip(path);
		PluginSettings::set(this, "lastDir", path);
		
		// add new current path for watching
		fsWatcher_.addPath(path);
	}
}

void FMPlugin::itemDoubleClicked(const QModelIndex& index) {
	QString path = model_->filePath(index);
	if ( QFileInfo(path).isDir() ) {
		cd(path);
	}
	else {
		api()->openDoc(path);
	}
}

void FMPlugin::onDirChanged(const QString&) {
}

void FMPlugin::back() {
	if ( !history_.isEmpty() ) {
		QString dir = history_.pop();
		if ( history_.isEmpty() )
			backBtn_->setEnabled(false);
		cd(dir, false);
	}
}

void FMPlugin::up() {
	// keep the current path
	QModelIndex curIndex = tree_->rootIndex();
	QString prevPath = model_->filePath(curIndex);
	
	QModelIndex parent = curIndex.parent();
	if ( parent.isValid() ) {
		cd(model_->filePath(parent));
		
		// select the previous dir
		QModelIndex prevIndex = model_->index(prevPath);
		if ( prevIndex.isValid() ) {
			tree_->setCurrentIndex(prevIndex);
		}
	}
}

void FMPlugin::home() {
	cd(QDir::homePath());
}

void FMPlugin::curFileDir() {
	Juff::Document* doc = api()->currentDocument();
	if ( !doc->isNull() && !doc->isNoname() ) {
		cd(QFileInfo(doc->fileName()).absolutePath());
	}
}

void FMPlugin::favorites() {
	favoritesMenu_->exec(QCursor::pos());
}

void FMPlugin::newDir() {
	QString newDirName = QInputDialog::getText(tree_, tr("New directory"), tr("Directory name"));
    if (newDirName.isEmpty())
        return;

    QDir curDir(model_->filePath(tree_->rootIndex()));
    if ( !curDir.mkdir(newDirName) ) {
        QMessageBox::warning(tree_, tr("Warning"),
                             tr("Couldn't create a dir named '%1'").arg(newDirName));
    }
}


void FMPlugin::goToFavorite() {
	QAction* a = qobject_cast<QAction*>(sender());
	if ( a ) {
		cd(a->text());
	}
}

void FMPlugin::addToFavorites() {
	QString path = model_->filePath(tree_->rootIndex());
	qDebug() << path;
	if ( !favorites_.contains(path) ) {
		favorites_.append(path);
		initFavoritesMenu();
		PluginSettings::set(this, "favorites", favorites_.join(";"));
	}
}

void FMPlugin::manageFavorites() {
	ManageDlg dlg(tree_, this);
	dlg.exec();
	QString favStr = PluginSettings::getString(this, "favorites");
	if ( !favStr.isEmpty() )
		favorites_ = favStr.split(";");
	else
		favorites_ = QStringList();
	initFavoritesMenu();
}

void FMPlugin::textEntered() {
	if ( QFileInfo(pathEd_->text()).isDir() )
		cd(pathEd_->text());
	else
		pathEd_->setText(model_->filePath(tree_->rootIndex()));
}

void FMPlugin::onDocSaved(const QString& fileName) {
    Q_UNUSED(fileName);
}

void FMPlugin::applySettings() {
	QPalette plt = tree_->palette();
	plt.setColor(QPalette::Base, EditorSettings::get(EditorSettings::DefaultBgColor));
	plt.setColor(QPalette::Text, EditorSettings::get(EditorSettings::DefaultFontColor));
//	plt.setColor(QPalette::Highlight, EditorSettings::get(EditorSettings::SelectionBgColor));
//	plt.setColor(QPalette::HighlightedText, TextDocSettings::selectionTextColor());
	tree_->setPalette(plt);
	pathEd_->setPalette(plt);

	PluginSettings::set(this, "ShowAsTree", showAsTree);
	tree_->setRootIsDecorated(showAsTree);
	tree_->setItemsExpandable(showAsTree);

    QDir::Filters filter = QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot;
    if (showHidden)
        filter |= QDir::Hidden;
    model_->setFilter(filter);
}

QWidget * FMPlugin::settingsPage() const
{
	QWidget * cfg = new QWidget();
	QVBoxLayout * l = new QVBoxLayout();
	cfg->setLayout(l);

	QCheckBox * treeCheckBox = new QCheckBox(tr("Show Directory Structure in a Tree"), cfg);
	treeCheckBox->setChecked(showAsTree);
	connect(treeCheckBox, SIGNAL(toggled(bool)), this, SLOT(treeCheckBox_toggled(bool)));

    QCheckBox *showHiddenBox = new QCheckBox(tr("Show Hidden Files and Directories"), cfg);
    showHiddenBox->setChecked(showHidden);
    connect(showHiddenBox, SIGNAL(toggled(bool)), this, SLOT(showHiddenBox_toggled(bool)));

	l->addWidget(treeCheckBox);
    l->addWidget(showHiddenBox);
	l->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
	return cfg;
}

void FMPlugin::treeCheckBox_toggled(bool value)
{
	showAsTree = value;
}

void FMPlugin::showHiddenBox_toggled(bool value)
{
    showHidden = value;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(fm, FMPlugin)
#endif
