#ifndef __JUFFED_TAB_BAR_H__
#define __JUFFED_TAB_BAR_H__

#include <QMenu>
#include <QTabBar>

namespace Juff {

class TabWidget;

class TabBar : public QTabBar {
Q_OBJECT
public:
	TabBar(Juff::TabWidget*);

signals:
	void requestTabClose(int);

protected slots:
//	void cloneDoc();
	void closeDoc();

protected:
	virtual void mouseReleaseEvent(QMouseEvent* e);

private:
	int index_;
	QMenu* tabMenu_;
	TabWidget* tabWidget_;
};

}; // namespace Juff

#endif // __JUFFED_TAB_BAR_H__
