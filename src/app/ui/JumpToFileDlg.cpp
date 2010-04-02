#include "JumpToFileDlg.h"

#include "Functions.h"
#include "Log.h"

#include <QKeyEvent>
#include <QListWidgetItem>

JumpToFileDlg::JumpToFileDlg(const QStringList& files, QWidget* parent) : QDialog(parent) {
	ui_.setupUi(this);
	
	foreach (QString fileName, files) {
		if ( !Juff::isNoname(fileName) ) {
			QListWidgetItem* item = new QListWidgetItem(Juff::docTitle(fileName, false));
			item->setData(Qt::UserRole + 1, fileName);
			ui_.fileList->addItem(item);
		}
	}
	ui_.fileList->setCurrentRow(0);
	
	connect(ui_.fileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(onItemDoubleClicked(QListWidgetItem*)));
	connect(ui_.fileNameEd, SIGNAL(textEdited(const QString&)), SLOT(onTextEdited(const QString&)));
	connect(ui_.fileNameEd, SIGNAL(returnPressed()), SLOT(onReturnPressed()));
	
	ui_.fileNameEd->installEventFilter(this);
	ui_.fileList->installEventFilter(this);
}

QString JumpToFileDlg::fileName() const {
	QListWidgetItem* item = ui_.fileList->currentItem();
	if ( item != 0 ) {
		return item->data(Qt::UserRole + 1).toString();
	}
	else {
		return "";
	}
}

void JumpToFileDlg::onItemDoubleClicked(QListWidgetItem*) {
	accept();
}

void JumpToFileDlg::onTextEdited(const QString& text) {
	LOGGER;
	
	int count = ui_.fileList->count();
	bool hasVisible = false;
	for (int i = 0; i < count; ++i) {
		QListWidgetItem* item = ui_.fileList->item(i);
		if ( item->text().contains(text, Qt::CaseInsensitive) ) {
			item->setHidden(false);
			if ( !hasVisible ) {
				ui_.fileList->setCurrentItem(item);
				hasVisible = true;
			}
		}
		else {
			item->setHidden(true);
		}
	}
	if ( !hasVisible )
		ui_.fileList->setCurrentItem(0);
}

void JumpToFileDlg::onReturnPressed() {
	LOGGER;
}

bool JumpToFileDlg::eventFilter(QObject *obj, QEvent *ev) {
	LOGGER;
	if ( obj == ui_.fileNameEd ) {
		if ( ev->type() == QEvent::KeyPress ) {
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);
			if ( keyEvent->key() == Qt::Key_Down ) {
				ui_.fileList->setFocus();
			}
		}
	}
	else if ( obj == ui_.fileList ) {
		if ( ev->type() == QEvent::KeyPress ) {
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);
			if ( keyEvent->key() == Qt::Key_Up && ui_.fileList->currentRow() == 0 ) {
				ui_.fileNameEd->setFocus();
			}
		}
	}
	return QDialog::eventFilter(obj, ev);
}
