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

#ifndef _TAB_BAR_H_
#define _TAB_BAR_H_

class QMenu;

#include <QtGui/QTabBar>

namespace Juff {
namespace GUI {

class TabBar : public QTabBar {
Q_OBJECT
public:
	TabBar(QWidget* parent);
	virtual ~TabBar();

signals:
	void tabCloseRequested(int);
	void requestFileName(int, QString&);
	void requestNextDoc();
	void requestPrevDoc();
	
protected slots:
	void copyFileName();
	void copyFilePath();
	void copyDirPath();

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);

private:
	QMenu* tabMenu_;
	int index_;
};

}	//	namespace GUI
}	//	namespace Juff

#endif
