#include "ProjectTree.h"

#include "DocHandlerInt.h"
#include "Functions.h"
#include "Log.h"

#include <QMenu>
#include <QFileInfo>
#include <QHeaderView>
#include <QTreeWidget>
#include <QVBoxLayout>

enum {
	ItemType = Qt::UserRole + 1,
	FilePath = Qt::UserRole + 2,
};

ProjectTree::ProjectTree(Juff::DocHandlerInt* handler) : QWidget() {
	docHandler_ = handler;
	
	prj_ = NULL;
	tree_ = new QTreeWidget(this);
	tree_->header()->hide();
	tree_->setContextMenuPolicy(Qt::CustomContextMenu);
	QVBoxLayout* vBox = new QVBoxLayout();
	vBox->setContentsMargins(0, 0, 0, 0);
	vBox->addWidget(tree_);
	setLayout(vBox);
	tree_->setRootIsDecorated(false);
	
	connect(tree_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
//	connect(tree_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(onItemClicked(QTreeWidgetItem*, int)));
	connect(tree_, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(onContextMenuRequested(const QPoint&)));
	
	prjMenu_ = new QMenu();
	fileMenu_ = new QMenu();
	
	fileMenu_->addAction(tr("Remove from project"), this, SLOT(onRemoveFromProject()));
}

void ProjectTree::setProject(Juff::Project* prj) {
	prj_ = prj;
	if ( tree_->parentWidget() != 0 )
		tree_->parentWidget()->setObjectName(prj_->name().isEmpty() ? tr("Documents") : prj_->name());
	
	connect(prj_, SIGNAL(fileAdded(const QString&)), SLOT(onFileAdded(const QString&)));
	connect(prj_, SIGNAL(fileRemoved(const QString&)), SLOT(onFileRemoved(const QString&)));
	
	updateTree();
}

void ProjectTree::updateTree() {
	tree_->clear();
	if ( prj_ == NULL )
		return;
	
	parsePrjItem(prj_, 0);
}

void ProjectTree::parsePrjItem(Juff::Project* prj, QTreeWidgetItem* parent) {
	if ( prj->hasSubProjects() ) {
		// subprojects
		int count = prj->subProjectCount();
		for ( int i = 0; i < count; ++i ) {
			Juff::Project* sub = prj->subProject(i);
			QTreeWidgetItem* subItem = new QTreeWidgetItem(QStringList() << sub->name());
			if ( parent != NULL )
				parent->addChild(subItem);
			else
				tree_->addTopLevelItem(subItem);
			
			subItem->setExpanded(true);
			subItem->setData(0, ItemType, "project");
			
			parsePrjItem(sub, subItem);
		}
	}
	else if ( prj->hasFiles() ) {
		// files
		QStringList files = prj->files();
		foreach (QString file, files) {
			QTreeWidgetItem* fileItem = new QTreeWidgetItem(QStringList() << Juff::docTitle(file, false));
			fileItem->setToolTip(0, file);
			if ( parent != 0 )
				parent->addChild(fileItem);
			else
				tree_->addTopLevelItem(fileItem);
			
			fileItem->setIcon(0, QIcon(":doc_icon"));
			fileItem->setData(0, ItemType, "file");
			fileItem->setData(0, FilePath, file);
		}
	}
}



void ProjectTree::onFileAdded(const QString&) {
	// TODO : add proper updating without rebuilding the whole tree
	updateTree();
}

void ProjectTree::onFileRemoved(const QString&) {
	// TODO : add proper updating without rebuilding the whole tree
	updateTree();
}

void ProjectTree::onItemDoubleClicked(QTreeWidgetItem* item, int) {
	if ( item == 0 )
		return;
	
	QString type = item->data(0, ItemType).toString();
	if ( type.compare("file") == 0 ) {
		QString path = item->data(0, FilePath).toString();
		openFile(path);
	}
}

void ProjectTree::onContextMenuRequested(const QPoint& point) {
	LOGGER;
	QTreeWidgetItem* item = tree_->itemAt(point);
	if ( item == 0 )
		return;
	
	QString type = item->data(0, ItemType).toString();
	if ( type == "project" ) {
		prjMenu_->popup(tree_->mapToGlobal(point));
	}
	else if ( type == "file" ) {
		fileMenu_->popup(tree_->mapToGlobal(point));
	}
}

void ProjectTree::onRemoveFromProject() {
	LOGGER;
	
	QTreeWidgetItem* item = tree_->currentItem();
	if ( item != 0 ) {
		prj_->removeFile(item->data(0, FilePath).toString());
	}
}

void ProjectTree::openFile(const QString& filePath) {
	docHandler_->openDoc(filePath);
}
