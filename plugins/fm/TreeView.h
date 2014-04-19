#ifndef _FM_TREE_VIEW_H_
#define _FM_TREE_VIEW_H_

class JuffPlugin;
class QMenu;

#include <QTreeView>

class TreeView : public QTreeView {
Q_OBJECT
public:
	TreeView(JuffPlugin*, QWidget* parent = 0);
	void initMenu();

signals:
	void goUp();

protected:
	virtual void keyPressEvent(QKeyEvent* e);
	virtual bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void showHideColumn();

private:
	void renameCurrent();

	JuffPlugin* plugin_;
	QMenu* headerMenu_;
};

#endif
