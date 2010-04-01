#include <QDebug>

#include "JuffMW.h"
#include "Log.h"

#include "Document.h"
#include "Functions.h"
#include "MessageWidget.h"
#include "SelectFilesDlg.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>

JuffMW::JuffMW() : QMainWindow() {
	resize(800, 700);

	mainWidget_ = new QWidget();
	vBox_ = new QVBoxLayout();
	vBox_->setContentsMargins(0, 0, 0, 0);
	mainWidget_->setLayout(vBox_);
	setCentralWidget(mainWidget_);
}

void JuffMW::setViewer(QWidget* w) {
	vBox_->addWidget(w);
}

QString JuffMW::getOpenFileName(const QString& filters) {
	// TODO : 
	QString dir = "";
	return QFileDialog::getOpenFileName(this, tr("Open file"), dir, filters);
}

QStringList JuffMW::getOpenFileNames(const QString& filters) {
	// TODO : 
	QString dir = "";
	return QFileDialog::getOpenFileNames(this, tr("Open files"), dir, filters);
}

QString JuffMW::getSaveFileName(const QString& curFileName, const QString& filters) {
	QString fileName;
	if ( !curFileName.isEmpty() && !Juff::isNoname(curFileName) ) {
		fileName = curFileName;
	}
	return QFileDialog::getSaveFileName(this, tr("Save %1 as...").arg(Juff::docTitle(curFileName, false)), fileName, filters);
}

QString JuffMW::getSavePrjName(const QString& title) {
	// TODO :
	QString dir = "";
	return QFileDialog::getSaveFileName(this, title, dir, "XML JuffEd Project Files (*.xml)");
}

int JuffMW::getGotoLineNumber(int lineCount) {
	bool ok = false;
	int line = QInputDialog::getInteger(this, tr("Go to line"), 
			tr("Go to line") + QString(" (1 - %1):").arg(lineCount), 
			1, 1, lineCount, 1, &ok);
	if ( ok )
		return line - 1;
	else
		return -1;
}

int JuffMW::askForSave(const QString& fileName) {
	QString str = tr("The document ") + fileName;
	str += tr(" has been modified.\nDo you want to save your changes?");
	return QMessageBox::warning(this, tr("Close document"),
			str, QMessageBox::Save | QMessageBox::Discard
			| QMessageBox::Cancel, QMessageBox::Save);
}

bool JuffMW::askForSave(const QStringList& filesIn, QStringList& filesOut) {
	LOGGER;
	
	filesOut.clear();
	
	SelectFilesDlg dlg(filesIn, this);
	if ( dlg.exec() == QDialog::Accepted ) {
		filesOut = dlg.checkedFiles();
		return true;
	}
	else {
		return false;
	}
}



////////////////////////////////////////////////////////////////////////////////
// Information display

void JuffMW::addStatusWidget(QWidget* w) {
	statusBar()->addWidget(w);
}

void JuffMW::message(const QIcon& icon, const QString& title, const QString& message, int timeout) {
	MessageWidget* msg = new MessageWidget(QIcon(), title, message, timeout, this);
	vBox_->insertWidget(0, msg);
	vBox_->setStretchFactor(msg, 0);
//	vBox_->addWidget(msg);
}



////////////////////////////////////////////////////////////////////////////////

void JuffMW::closeEvent(QCloseEvent* e) {
	LOGGER;
	
	bool confirmed = true;
	emit closeRequested(confirmed);
	if ( confirmed ) {
//		MainSettings::setWindowRect(geometry());
		e->accept();
	}
	else {
		e->ignore();
	}
}
