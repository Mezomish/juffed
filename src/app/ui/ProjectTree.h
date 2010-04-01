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
