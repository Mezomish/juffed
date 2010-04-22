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

#include "TabBar.h"

#include "Log.h"
#include "TabWidget.h"

#include <QMouseEvent>

namespace Juff {

TabBar::TabBar(Juff::TabWidget* widget) : QTabBar(), index_(-1), tabMenu_(NULL), tabWidget_(widget) {
	setFocusPolicy(Qt::NoFocus);
	
#if QT_VERSION >= 0x040500
	setMovable(true);
#endif
}

void TabBar::mouseReleaseEvent(QMouseEvent* e) {

	if ( e->button() & Qt::MidButton ) {
		int index = tabAt(e->pos());
		emit requestTabClose(index);
	}
	else if ( e->button() & Qt::RightButton ) {
		index_ = tabAt(e->pos());
		if ( tabMenu_ != NULL )
			delete tabMenu_;
		
		tabMenu_ = new QMenu();
		tabWidget_->initDocMenu(index_, tabMenu_);
		
		if ( !tabMenu_->isEmpty() )
			tabMenu_->addSeparator();
		
		tabMenu_->addAction(tr("Close"), this, SLOT(closeDoc()));
		
		tabMenu_->popup(e->globalPos());
	}
	
	QTabBar::mouseReleaseEvent(e);
}


/*void TabBar::cloneDoc() {
}*/

void TabBar::closeDoc() {
	LOGGER;
	
	emit requestTabClose(index_);
}

} // namespace Juff
