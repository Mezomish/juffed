#include <QDebug>

#include "TreeView.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QHeaderView>
#include <QMenu>
#include <QFileSystemModel>

#include <PluginSettings.h>

TreeView::TreeView(JuffPlugin* plugin, QWidget* parent) 
: QTreeView(parent)
, plugin_(plugin)
, headerMenu_(NULL)
{
	header()->installEventFilter(this);
}

void TreeView::initMenu() {
	headerMenu_ = new QMenu(this);
	int hCount = header()->count();
	for ( int i = 1; i < hCount; ++i ) {
		QString title = model()->headerData(i, Qt::Horizontal).toString();
		QAction* a = headerMenu_->addAction(title, this, SLOT(showHideColumn()));
		a->setData(i);
		a->setCheckable(true);
		bool visible = PluginSettings::getBool(plugin_, QString("column%1").arg(i));
		if ( visible )
			a->setChecked(true);
		else
			setColumnHidden(i, true);
	}
}

void TreeView::keyPressEvent(QKeyEvent* e) {
	if ( e->modifiers() == Qt::NoModifier ) {
		int key = e->key();
		switch ( key ) {
			case Qt::Key_Backspace :
				emit goUp();
				break;
			
			case Qt::Key_Return :
			case Qt::Key_Enter :
				emit doubleClicked(currentIndex());
				setFocus();
				break;
			
			case Qt::Key_F2 :
				renameCurrent();
				break;
			
			default: ;
		}
	}
	else if ( e->modifiers() == Qt::AltModifier ) {
		if ( e->key() == Qt::Key_Up ) {
			emit goUp();
			return;
		}
	}
	QTreeView::keyPressEvent(e);
}

void TreeView::renameCurrent() {
    QFileSystemModel* dirModel = qobject_cast<QFileSystemModel*>(model());
	if ( 0 != dirModel ) {
		QFileInfo fi = dirModel->fileInfo(currentIndex());
		QString newFileName = QInputDialog::getText(this, tr("Rename"), tr("File name"), 
		                                            QLineEdit::Normal, fi.fileName());
        if (newFileName.isEmpty())
            return;

        QFile file(fi.absoluteFilePath());
        QDir::setCurrent(fi.absolutePath());
        if ( !file.rename(newFileName) ) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Rename failed: file '%1' already exists").arg(newFileName));
        }
	}
}

bool TreeView::eventFilter(QObject *obj, QEvent *event) {
	if ( obj == header() && event->type() == QEvent::ContextMenu ) {
		headerMenu_->exec(QCursor::pos());
		return true;
	}
	return false;
}

void TreeView::showHideColumn() {
	QAction* a = qobject_cast<QAction*>(sender());
	if ( 0 != a ) {
		int col = a->data().toInt();
		if ( col >= 0 ) {
			bool curHidden = isColumnHidden(col);
			setColumnHidden(col, !curHidden);
			PluginSettings::set(plugin_, QString("column%1").arg(col), curHidden);
		}
	}
}

