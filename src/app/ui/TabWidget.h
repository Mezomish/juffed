#ifndef __JUFFED_TAB_WIDGET_H__
#define __JUFFED_TAB_WIDGET_H__

class QMenu;

#include <QTabWidget>

namespace Juff {

class Document;
class DocListButton;
class DocHandlerInt;

class TabWidget : public QTabWidget {
Q_OBJECT
public:
	TabWidget(Juff::DocHandlerInt*);
	
	void initDocMenu(int index, QMenu* menu);

signals:
	void requestDocMove(Juff::Document*, Juff::TabWidget*);
	void tabRemoved(Juff::TabWidget*);
	void docStackCalled(bool forward);

private slots:
	void onDocListNeedsToBeShown();
	void onDocMenuItemSelected();
	void onTabCloseRequested(int);
	void onCloseAllRequested();
	void onCloseAllOtherRequested(int);

	void slotMoveDoc();
	void slotCopyFileName();
	void slotCopyFilePath();
	void slotCopyDirPath();
	
protected:
	virtual void tabRemoved(int);
	virtual void tabInserted(int);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void dragEnterEvent(QDragEnterEvent*);
	virtual void dropEvent(QDropEvent*);
	virtual void contextMenuEvent(QContextMenuEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);

private:
	QString docName(int index) const;
	
	Juff::DocHandlerInt* handler_;
	int menuRequestedIndex_;
	int selfIndex_;
	DocListButton* docListBtn_;
	QMenu* contextMenu_;
};

} // namespace Juff

#endif // __JUFFED_TAB_WIDGET_H__
