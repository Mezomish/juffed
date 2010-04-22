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

	void initDocMenu(int, QMenu*);

signals:
//	void requestDocClone(Juff::Document*, Juff::TabWidget*);
	void requestDocMove(Juff::Document*, Juff::TabWidget*);
	void tabRemoved(Juff::TabWidget*);
	void docStackCalled(bool forward);

protected slots:
//	void cloneDoc();
	void moveDoc();
	void copyFileName();
	void copyFilePath();
	void copyDirPath();
	void addFileToProject();
	void removeFileFromProject();
//	void onDocListPressed();

protected:
	virtual void tabRemoved(int);
	virtual void tabInserted(int);
	virtual void dragEnterEvent(QDragEnterEvent*);
	virtual void dropEvent(QDropEvent*);
	virtual void keyPressEvent(QKeyEvent*);

private slots:
	void onTabCloseRequested(int);
	void onDocListNeedsToBeShown();
	void onDocMenuItemSelected();

private:
	QString docName(int) const;

	Juff::DocHandlerInt* handler_;
	int menuRequestedIndex_;
	int selfIndex_;
	DocListButton* docListBtn_;
};

}; // namespace Juff

#endif // __JUFFED_TAB_WIDGET_H__
