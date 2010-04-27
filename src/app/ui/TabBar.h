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
