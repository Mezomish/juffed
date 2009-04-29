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

#include "TabBar.h"

//	Qt headers
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

//	local headers
#include "Log.h"

namespace Juff {
namespace GUI {

TabBar::TabBar(QWidget* parent) : QTabBar(parent), index_(-1) {
	tabMenu_ = new QMenu();
	tabMenu_->addAction(tr("Copy file name to clipboard"), this, SLOT(copyFileName()));
	tabMenu_->addAction(tr("Copy full file path to clipboard"), this, SLOT(copyFilePath()));
	tabMenu_->addAction(tr("Copy file directory path to clipboard"), this, SLOT(copyDirPath()));
	tabMenu_->addSeparator();
	tabMenu_->addAction(tr("Close"), this, SLOT(closeTab()));

#if QT_VERSION >= 0x040500
	setMovable(true);
#endif
}

TabBar::~TabBar() {
	JUFFDTOR;

	delete tabMenu_;
}

void TabBar::mouseReleaseEvent(QMouseEvent* e) {

#if QT_VERSION >= 0x040300
	//	The QTabBar::tabAt() function was introduced 
	//	in Qt 4.3, that's why we need this #if ...
	if ( e->button() & Qt::MidButton ) {
		int index = tabAt(e->pos());
		emit requestTabClose(index);
	}
	else if ( e->button() & Qt::RightButton ) {
		index_ = tabAt(e->pos());
		tabMenu_->popup(e->globalPos());
	}
	
#endif

	QTabBar::mouseReleaseEvent(e);
}

void TabBar::wheelEvent(QWheelEvent* e) {
	if ( e->delta() < 0 ) {
		emit requestNextDoc();
	}
	else {
		emit requestPrevDoc();
	}
}

void TabBar::copyFileName() {
	QString fileName;
	requestFileName(index_, fileName);
	if ( !fileName.isEmpty() ) {
		QString name = QFileInfo(fileName).fileName();
		QApplication::clipboard()->setText(name);
	}
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* e) {

#if QT_VERSION >= 0x040300
	//	The QTabBar::tabAt() function was introduced 
	//	in Qt 4.3, that's why we need this #if ...
	int index = tabAt(e->pos());
	emit requestTabClose(index);
#endif

	QTabBar::mouseDoubleClickEvent(e);
}

void TabBar::copyFilePath() {
	QString fileName;
	requestFileName(index_, fileName);
	if ( !fileName.isEmpty() ) {
		QApplication::clipboard()->setText(fileName);
	}
}

void TabBar::copyDirPath() {
	QString fileName;
	requestFileName(index_, fileName);
	if ( !fileName.isEmpty() ) {
		QString name = QFileInfo(fileName).absolutePath();
		QApplication::clipboard()->setText(name);
	}
}

void TabBar::closeTab() {
	emit requestTabClose(index_);
}

}	//	namespace GUI
}	//	namespace Juff
