/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <QDebug>
#include "ProjectTree.h"

#include "CommandStorage.h"
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
	setWindowTitle(tr("Project"));
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
	
	fileMenu_->setDefaultAction(fileMenu_->addAction(tr("Open file"), this, SLOT(onOpenFile())));
	fileMenu_->addAction(tr("Remove from project"), this, SLOT(onRemoveFromProject()));
	
	prjMenu_->addAction(CommandStorage::instance()->action(Juff::PrjAddFile));
	prjMenu_->addAction(CommandStorage::instance()->action(Juff::PrjClose));
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
	
	if ( !prj_->name().isEmpty() ) {
		QTreeWidgetItem* root = new QTreeWidgetItem(QStringList() << prj_->name());
		tree_->addTopLevelItem(root);
		root->setData(0, ItemType, "project");
		root->setExpanded(true);
		parsePrjItem(prj_, root);
	}
	else
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
			
			QString ext = QFileInfo(file).suffix().toLower();
			if ( ext == "h" )
				fileItem->setIcon(0, QIcon(":file_h"));
			else if ( ext == "cpp" )
				fileItem->setIcon(0, QIcon(":file_cpp"));
			else
				fileItem->setIcon(0, QIcon(":file_generic"));
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


// Context menu actions

void ProjectTree::onRemoveFromProject() {
	LOGGER;
	
	QTreeWidgetItem* item = tree_->currentItem();
	if ( item != 0 ) {
		QString type = item->data(0, ItemType).toString();
		if ( type.compare("file") == 0 ) {
			QString path = item->data(0, FilePath).toString();
			prj_->removeFile(path);
		}
	}
}

void ProjectTree::onOpenFile() {
	LOGGER;
	
	QTreeWidgetItem* item = tree_->currentItem();
	if ( item != 0 ) {
		QString type = item->data(0, ItemType).toString();
		if ( type.compare("file") == 0 ) {
			QString path = item->data(0, FilePath).toString();
			openFile(path);
		}
	}
}



void ProjectTree::openFile(const QString& filePath) {
	docHandler_->openDoc(filePath);
}
