/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

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

#ifndef __JUFF_TAB_WIDGET_H__
#define __JUFF_TAB_WIDGET_H__

class QPushButton;

//	Qt headers
#include <QtGui/QTabWidget>

//	local headers
#include "TabBar.h"

namespace Juff {
namespace GUI {

class TabWidget : public QTabWidget {
Q_OBJECT
public:
	TabWidget(QWidget* parent);
	virtual ~TabWidget();

	void setCloseBtnOnTabs(bool);
	void enableCloseButton(bool);

public slots:
	void nextWidget();
	void prevWidget();

signals:
	void tabCloseRequested(int);
	void requestFileName(int, QString&);
	void newFileRequested();
	void docOpenRequested(const QString&);

#if QT_VERSION >= 0x040500
	void tabMoved(int from, int to);
#endif

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void dragEnterEvent(QDragEnterEvent*);
	virtual void dropEvent(QDropEvent*);
	virtual void contextMenuEvent(QContextMenuEvent*);

protected slots:
	void closeBtnPressed();

private:
	TabBar* tabBar_;
	QPushButton* closeBtn_;
	QMenu* contextMenu_;
};

}	//	namespace GUI
}	//	namespace Juff

#endif // __JUFF_TAB_WIDGET_H__
