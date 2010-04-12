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

#ifndef __JUFFED_PROJECT_TREE_H__
#define __JUFFED_PROJECT_TREE_H__

namespace Juff {
	class DocHandlerInt;
}

class QMenu;
class QTreeWidget;
class QTreeWidgetItem;

#include "Project.h"

#include <QWidget>

class ProjectTree : public QWidget {
Q_OBJECT
public:
	ProjectTree(Juff::DocHandlerInt*);

	void setProject(Juff::Project*);
	void updateTree();

private slots:
	void onFileAdded(const QString&);
	void onFileRemoved(const QString&);
	void onItemDoubleClicked(QTreeWidgetItem*, int);
	void onContextMenuRequested(const QPoint&);
	
	void onOpenFile();
	void onRemoveFromProject();

private:
	void openFile(const QString&);

	void parsePrjItem(Juff::Project*, QTreeWidgetItem*);

	Juff::DocHandlerInt* docHandler_;
	Juff::Project* prj_;
	QTreeWidget* tree_;
	QMenu* prjMenu_;
	QMenu* fileMenu_;
};

#endif // __JUFFED_PROJECT_TREE_H__
