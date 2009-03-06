/*
JuffEd - A simple text editor
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

#include "TabWidget.h"

#include "Log.h"

namespace Juff {
namespace GUI {

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent) {
	tabBar_ = new TabBar(this);
	setTabBar(tabBar_);
	tabBar()->setFocusPolicy(Qt::NoFocus); 
	setAcceptDrops(true);
	
	connect(tabBar_, SIGNAL(tabCloseRequested(int)), this, SIGNAL(tabCloseRequested(int)));
	connect(tabBar_, SIGNAL(requestFileName(int, QString&)), this, SIGNAL(requestFileName(int, QString&)));
	connect(tabBar_, SIGNAL(requestNextDoc()), this, SLOT(nextWidget()));
	connect(tabBar_, SIGNAL(requestPrevDoc()), this, SLOT(prevWidget()));
}

void TabWidget::nextWidget() {
	int index = currentIndex();
	if ( index + 1 < count() )
		setCurrentIndex(index + 1);
}

void TabWidget::prevWidget() {
	int index = currentIndex();
	if ( index - 1 >= 0 )
		setCurrentIndex(index - 1);
}

}	//	namespace GUI
}	//	namespace Juff
