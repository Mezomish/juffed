#ifndef __JUFFED_TAB_WIDGET_H__
#define __JUFFED_TAB_WIDGET_H__

class QMenu;

#include <QTabWidget>

namespace Juff {

class Document;
class DocListButton;

class TabWidget : public QTabWidget {
Q_OBJECT
public:
	TabWidget();

	void initDocMenu(int, QMenu*);

signals:
	void requestDocClose(Juff::Document*, Juff::TabWidget*);
	void requestDocClone(Juff::Document*, Juff::TabWidget*);
	void requestDocMove(Juff::Document*, Juff::TabWidget*);
	void tabRemoved(Juff::TabWidget*);

protected slots:
//	void cloneDoc();
	void moveDoc();
	void copyFileName();
	void copyFilePath();
	void copyDirPath();
//	void onDocListPressed();

protected:
	virtual void tabRemoved(int);
	virtual void tabInserted(int);

private slots:
	void onTabCloseRequested(int);
	void onDocListNeedsToBeShown();
	void onDocMenuItemSelected();

private:
	QString docName(int) const;

	int menuRequestedIndex_;
	int index_;
	DocListButton* docListBtn_;
};

}; // namespace Juff

#endif // __JUFFED_TAB_WIDGET_H__
