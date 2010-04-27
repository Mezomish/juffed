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

#include "TabWidget.h"

#include "Document.h"
#include "DocHandlerInt.h"
#include "Functions.h"
#include "IconManager.h"
#include "Log.h"
#include "Project.h"
#include "TabBar.h"

#include <QApplication>
#include <QClipboard>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMenu>
#include <QPushButton>
#include <QUrl>

namespace Juff {

class DocListButton : public QPushButton {
public:
	DocListButton(const QIcon& icon) : QPushButton(icon, "") {
		setMinimumSize(sz, sz);
		setMaximumSize(sz, sz);
		setFlat(true);
		
		menu_ = new QMenu();
		setMenu(menu_);
	}
	
	virtual QSize sizeHint() const {
		return QSize(sz, sz);
	}
	
	static const int sz;
	QMenu* menu_;
};

const int DocListButton::sz = 24;

TabWidget::TabWidget(Juff::DocHandlerInt* handler) : QTabWidget() {
	handler_ = handler;
	
	setTabBar(new Juff::TabBar(this));
	connect(tabBar(), SIGNAL(tabCloseRequested(int)), SLOT(onTabCloseRequested(int)));
	setAcceptDrops(true);
	
	static int ind = 0;
	selfIndex_ = ind++;
	
	docListBtn_ = new DocListButton(QIcon());
	docListBtn_->setToolTip(tr("Documents list"));
	setCornerWidget(docListBtn_, Qt::TopLeftCorner);
	docListBtn_->hide();
	
	connect(docListBtn_->menu(), SIGNAL(aboutToShow()), SLOT(onDocListNeedsToBeShown()));
}

QString TabWidget::docName(int index) const {
	if ( index < 0 )
		return "";
	
	QWidget* tab = widget(index);
	Juff::Document* doc = qobject_cast<Juff::Document*>(tab);
	if ( doc != 0 )
		return doc->fileName();
	else
		return "";
}

void TabWidget::initDocMenu(int index, QMenu* menu) {
	LOGGER;
	
	QWidget* tab = widget(index);
	Juff::Document* doc = qobject_cast<Juff::Document*>(tab);
	if ( doc != 0 ) {
		menuRequestedIndex_ = index;
		
		menu->addAction(tr("Copy file name to clipboard"), this, SLOT(copyFileName()));
		if ( !Juff::isNoname(doc) ) {
			menu->addAction(tr("Copy full file path to clipboard"), this, SLOT(copyFilePath()));
			menu->addAction(tr("Copy file directory path to clipboard"), this, SLOT(copyDirPath()));
		}
		
		menu->addSeparator();
//		if ( doc->supportsAction(Juff::FileClone) && !doc->hasClone() )
//			menu->addAction(tr("Clone to another panel"), this, SLOT(cloneDoc()));
		if ( selfIndex_ == 0 )
			menu->addAction(tr("Move to the right panel"), this, SLOT(moveDoc()));
		else
			menu->addAction(tr("Move to the left panel"), this, SLOT(moveDoc()));
		
		Juff::Project* prj = handler_->curPrj();
		// TODO : add an accurate check whether the file is a part of the project
		if ( prj != 0 && !prj->isNoname() ) {
			if ( !prj->files().contains(doc->fileName()) )
				menu->addAction(tr("Add to project"), this, SLOT(addFileToProject()))->setIcon(IconManager::instance()->icon(Juff::PrjAddFile));
			else
				menu->addAction(tr("Remove from project"), this, SLOT(removeFileFromProject()))->setIcon(IconManager::instance()->icon(Juff::PrjRemoveFile));
		}
	}
	else {
		menuRequestedIndex_ = -1;
	}
}

void TabWidget::copyFileName() {
	QString fileName = docName(menuRequestedIndex_);
	if ( !fileName.isEmpty() ) {
		QString name = QFileInfo(fileName).fileName();
		QApplication::clipboard()->setText(name);
	}
}

void TabWidget::copyFilePath() {
	QString fileName = docName(menuRequestedIndex_);
	if ( !fileName.isEmpty() ) {
		QApplication::clipboard()->setText(fileName);
	}
}

void TabWidget::copyDirPath() {
	QString fileName = docName(menuRequestedIndex_);
	if ( !fileName.isEmpty() ) {
		QString name = QFileInfo(fileName).absolutePath();
		QApplication::clipboard()->setText(name);
	}
}

void TabWidget::addFileToProject() {
	QString fileName = docName(menuRequestedIndex_);
	if ( !fileName.isEmpty() ) {
		Juff::Project* prj = handler_->curPrj();
		prj->addFile(fileName);
	}
}

void TabWidget::removeFileFromProject() {
	QString fileName = docName(menuRequestedIndex_);
	if ( !fileName.isEmpty() ) {
		Juff::Project* prj = handler_->curPrj();
		prj->removeFile(fileName);
	}
}

void TabWidget::onTabCloseRequested(int index) {
	LOGGER;
	
	QWidget* tab = widget(index);
	Juff::Document* doc = qobject_cast<Juff::Document*>(tab);
	if ( doc != 0 ) {
		handler_->closeDoc(doc->fileName());
	}
}

void TabWidget::onDocListNeedsToBeShown() {
	LOGGER;
	
	docListBtn_->menu_->clear();
	int n = count();
	for ( int i = 0; i < n; ++i ) {
		Juff::Document* doc = qobject_cast<Juff::Document*>(widget(i));
		if ( doc != 0 ) {
			docListBtn_->menu_->addAction(Juff::docIcon(doc), Juff::docTitle(doc), this, SLOT(onDocMenuItemSelected()))->setData(i);
		}
	}
}

void TabWidget::onDocMenuItemSelected() {
	LOGGER;
	
	QAction* act = qobject_cast<QAction*>(sender());
	if ( act != 0 ) {
		setCurrentIndex(act->data().toInt());
	}
}

//void TabWidget::onDocListPressed() {
//	LOGGER;
//	docListBtn_->men
//}

void TabWidget::tabInserted(int) {
	LOGGER;
	
	if ( count() > 0 )
		docListBtn_->show();
}

void TabWidget::tabRemoved(int) {
	LOGGER;
	
	emit tabRemoved(this);
	
	if ( count() == 0 )
		docListBtn_->hide();
}


/*void TabWidget::cloneDoc() {
	LOGGER;
	
	QWidget* tab = widget(menuRequestedIndex_);
	Juff::Document* doc = qobject_cast<Juff::Document*>(tab);
	if ( doc != 0 ) {
		emit requestDocClone(doc, this);
	}
}*/

void TabWidget::moveDoc() {
	LOGGER;
	
	QWidget* tab = widget(menuRequestedIndex_);
	Juff::Document* doc = qobject_cast<Juff::Document*>(tab);
	if ( doc != 0 ) {
		emit requestDocMove(doc, this);
	}
}


void TabWidget::dragEnterEvent(QDragEnterEvent* e) {
	LOGGER;

	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void TabWidget::dropEvent(QDropEvent* e) {
	LOGGER;

	if ( e->mimeData()->hasUrls() ) {
		QList<QUrl> urls = e->mimeData()->urls();
		foreach (QUrl url, urls) {
			QString name = url.path();

#ifdef Q_OS_WIN32
			//	hack to protect of strings with filenames like /C:/doc/file.txt
			if ( name[0] == '/' )
				name.remove(0, 1);
#endif

			if ( !name.isEmpty() ) {
				handler_->openDoc(name);
			}
		}
	}
}

void TabWidget::keyPressEvent(QKeyEvent* e) {
	if ( e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab ) {
		if ( e->modifiers() & Qt::ControlModifier ) {
			if ( e->modifiers() & Qt::ShiftModifier ) {
				emit docStackCalled(false);
			}
			else {
				emit docStackCalled(true);
			}
			return;
		}
	}
	
	QTabWidget::keyPressEvent(e);
}

} // namespace Juff
