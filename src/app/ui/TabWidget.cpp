#include "TabWidget.h"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMenu>
#include <QPushButton>
#include <QUrl>

#include "CommandStorage.h"
#include "Constants.h"
#include "Document.h"
#include "DocHandlerInt.h"
#include "Enums.h"
#include "Log.h"
#include "MainSettings.h"
#include "TabBar.h"
#include "Utils.h"

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
	connect(tabBar(), SIGNAL(closeAllRequested()), SLOT(onCloseAllRequested()));
	connect(tabBar(), SIGNAL(closeAllOtherRequested(int)), SLOT(onCloseAllOtherRequested(int)));
	setAcceptDrops(true);
	
	static int ind = 0;
	selfIndex_ = ind++;
	
	docListBtn_ = new DocListButton(QIcon());
	docListBtn_->setToolTip(tr("Documents list"));
	setCornerWidget(docListBtn_, Qt::TopLeftCorner);
	docListBtn_->hide();
	
	connect(docListBtn_->menu(), SIGNAL(aboutToShow()), SLOT(onDocListNeedsToBeShown()));
	
	contextMenu_ = new QMenu();
	contextMenu_->addAction(Utils::commandStorage()->action(FILE_NEW));
	contextMenu_->addAction(tr("Close all"), this, SLOT(onCloseAllRequested()));
}

void TabWidget::initDocMenu(int index, QMenu* menu) {
	LOGGER;
	
	QWidget* tab = widget(index);
	Juff::Document* doc = qobject_cast<Juff::Document*>(tab);
	if ( doc != 0 ) {
		menuRequestedIndex_ = index;
		
		menu->addAction(tr("Copy file name to clipboard"), this, SLOT(slotCopyFileName()));
		if ( !doc->isNoname() ) {
			menu->addAction(tr("Copy full file path to clipboard"), this, SLOT(slotCopyFilePath()));
			menu->addAction(tr("Copy file directory path to clipboard"), this, SLOT(slotCopyDirPath()));
		}
		
		menu->addSeparator();
//		if ( doc->supportsAction(Juff::FileClone) && !doc->hasClone() )
//			menu->addAction(tr("Clone to another panel"), this, SLOT(cloneDoc()));
		if ( selfIndex_ == 0 )
			menu->addAction(QIcon(":arrow_right"), tr("Move to the right panel"), this, SLOT(slotMoveDoc()));
		else
			menu->addAction(QIcon(":arrow_left"), tr("Move to the left panel"), this, SLOT(slotMoveDoc()));
		
/*		Juff::Project* prj = handler_->curPrj();
		// TODO : add an accurate check whether the file is a part of the project
		if ( prj != 0 && !prj->isNoname() ) {
			if ( !prj->files().contains(doc->fileName()) )
				menu->addAction(tr("Add to project"), this, SLOT(addFileToProject()))->setIcon(IconManager::instance()->icon(Juff::PrjAddFile));
			else
				menu->addAction(tr("Remove from project"), this, SLOT(removeFileFromProject()))->setIcon(IconManager::instance()->icon(Juff::PrjRemoveFile));
		}*/
	}
	else {
		menuRequestedIndex_ = -1;
	}
}

void TabWidget::tabInserted(int) {
//	LOGGER;
	
	if ( count() > 0 )
		docListBtn_->show();
}

void TabWidget::tabRemoved(int) {
	LOGGER;
	
	emit tabRemoved(this);
	
	if ( count() == 0 )
		docListBtn_->hide();
}

void TabWidget::slotMoveDoc() {
	LOGGER;
	
	QWidget* tab = widget(menuRequestedIndex_);
	Juff::Document* doc = qobject_cast<Juff::Document*>(tab);
	if ( doc != 0 ) {
		emit requestDocMove(doc, this);
	}
}

void TabWidget::slotCopyFileName() {
	QString fileName = docName(menuRequestedIndex_);
	if ( !fileName.isEmpty() ) {
		QString name = QFileInfo(fileName).fileName();
		QApplication::clipboard()->setText(name);
	}
}

void TabWidget::slotCopyFilePath() {
	QString fileName = docName(menuRequestedIndex_);
	if ( !fileName.isEmpty() ) {
		QApplication::clipboard()->setText(fileName);
	}
}

void TabWidget::slotCopyDirPath() {
	QString fileName = docName(menuRequestedIndex_);
	if ( !fileName.isEmpty() ) {
		QString name = QFileInfo(fileName).absolutePath();
		QApplication::clipboard()->setText(name);
	}
}



void TabWidget::onDocListNeedsToBeShown() {
	LOGGER;
	
	docListBtn_->menu_->clear();
	int n = count();
	for ( int i = 0; i < n; ++i ) {
		Juff::Document* doc = qobject_cast<Juff::Document*>(widget(i));
		if ( doc != 0 ) {
			docListBtn_->menu_->addAction(doc->icon(), doc->title(), this, SLOT(onDocMenuItemSelected()))->setData(i);
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

void TabWidget::onTabCloseRequested(int index) {
	LOGGER;
	
	QWidget* tab = widget(index);
	Juff::Document* doc = qobject_cast<Juff::Document*>(tab);
	if ( doc != 0 ) {
		handler_->closeDoc(doc->fileName());
	}
}

void TabWidget::onCloseAllRequested() {
	if ( selfIndex_ == 0 )
		handler_->closeAllDocs(Juff::PanelLeft);
	else if ( selfIndex_ == 1 )
		handler_->closeAllDocs(Juff::PanelRight);
}

void TabWidget::onCloseAllOtherRequested(int index) {
	if ( selfIndex_ == 0 )
		handler_->closeAllOtherDocs(index, Juff::PanelLeft);
	else if ( selfIndex_ == 1 )
		handler_->closeAllOtherDocs(index, Juff::PanelRight);
}

void TabWidget::keyPressEvent(QKeyEvent* e) {
	if ( MainSettings::get(MainSettings::UseCtrlTabMenu) && (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) ) {
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

void TabWidget::contextMenuEvent(QContextMenuEvent* e) {
	if ( tabBar()->tabAt(tabBar()->mapFromParent(e->pos())) < 0 ) {
		contextMenu_->exec(e->globalPos());
	}
}

void TabWidget::mouseDoubleClickEvent(QMouseEvent* e) {
	if ( e->button() == Qt::LeftButton ) {
		// need to set focus to this tab widget, otherwise 
		// new doc will be created at active tab widget
		// that can be other than 'this' object
		currentWidget()->setFocus();
		
		Utils::commandStorage()->action(FILE_NEW)->trigger();
	}
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


} // namespace Juff
