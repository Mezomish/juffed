#include <QDebug>

#include "SearchDlg.h"

#include <QCompleter>
#include <QFileDialog>
#include <QFileSystemModel>


SearchDlg::SearchDlg(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);
	
    QCompleter *completer = new QCompleter(this);
    model = new QFileSystemModel(completer);
    model->setRootPath("");
    completer->setModel(model);
    ui.dirEd->setCompleter(completer);
	
	connect(ui.browseBtn, SIGNAL(clicked()), SLOT(slotBrowse()));
	
	ui.findInFilesFrame->hide();
}

void SearchDlg::slotBrowse() {
	QString dir = ui.dirEd->text();
	dir = QFileDialog::getExistingDirectory(parentWidget(), tr("Browse"), dir);
    if ( !dir.isEmpty() ) {
		ui.dirEd->setText(dir);
        model->setRootPath(dir);
    }
}


// getters

QString SearchDlg::findText() const {
	return ui.findEd->text();
}
bool SearchDlg::searchInFiles() const {
	return ui.findInFilesBtn->isChecked();
}
QString SearchDlg::startDir() const {
	return ui.dirEd->text();
}
bool SearchDlg::recursive() const {
	return ui.recursiveChk->isChecked();
}
QString SearchDlg::filePatterns() const {
	return ui.patternsEd->text();
}
int SearchDlg::patternVariant() const {
	if ( ui.inclFilesBtn->isChecked() ) {
		return 1;
	}
	else if ( ui.exclFilesBtn->isChecked() ) {
		return 2;
	}
	else if ( ui.allFilesBtn->isChecked() ) {
		return 0;
	}
	else {
		return -1;
	}
}


// setters

void SearchDlg::setFindText(const QString& text) {
	ui.findEd->setText(text);
}
void SearchDlg::setStartDir(const QString& dir) {
	ui.dirEd->setText(dir);
    model->setRootPath(dir);
}
void SearchDlg::setSearchInFiles(bool inFiles) {
	if ( inFiles ) {
		ui.findInFilesBtn->setChecked(true);
		ui.findInFilesFrame->hide();
	}
	else {
		ui.findInDirBtn->setChecked(true);
		ui.findInFilesFrame->show();
	}
}
void SearchDlg::setRecursive(bool rec) {
	ui.recursiveChk->setChecked(rec);
}
void SearchDlg::setFilePatterns (const QString& patterns) {
	ui.patternsEd->setText(patterns);
}
void SearchDlg::setPatternVariant(int var) {
	switch ( var ) {
		case 1 : ui.inclFilesBtn->setChecked(true); break;
		case 2 : ui.exclFilesBtn->setChecked(true); break;
		default : ui.allFilesBtn->setChecked(true);
	}
}
