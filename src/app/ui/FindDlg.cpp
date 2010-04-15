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

#include "FindDlg.h"

//	Qt headers
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>

#include "IconManager.h"
#include "MainSettings.h"

QString FindDlg::lastString_ = "";
QString FindDlg::lastReplaceText_ = "";
bool FindDlg::matchCase_ = true;
bool FindDlg::backward_ = false;
bool FindDlg::regExpMode_ = false;
bool FindDlg::wholeWords_ = false;
bool FindDlg::multiLine_ = false;
QStringList FindDlg::strings_;
QStringList FindDlg::replaces_;

FindDlg::FindDlg(QWidget* parent, bool repl) : 
	QDialog(parent) {

	uiFind.setupUi(this);

	foreach (QString str, strings_)
		uiFind.findCmb->addItem(str);
	foreach (QString repl, replaces_)
		uiFind.replaceCmb->addItem(repl);

	uiFind.findCmb->setCompleter(0);
	uiFind.replaceCmb->setCompleter(0);

	connect(uiFind.findBtn, SIGNAL(clicked()), SLOT(accept()));
	connect(uiFind.cancelBtn, SIGNAL(clicked()), SLOT(reject()));
	connect(uiFind.replaceChk, SIGNAL(toggled(bool)), SLOT(setReplaceMode(bool)));
	connect(uiFind.multiLineChk, SIGNAL(toggled(bool)), SLOT(multiLineChecked(bool)));
	connect(uiFind.regexpChk, SIGNAL(toggled(bool)), SLOT(regExpChecked(bool)));

	uiFind.findCmb->setEditText(lastString_);
	uiFind.mlEd->setText(lastString_);
	uiFind.replaceCmb->setEditText(lastReplaceText_);
	uiFind.matchCaseChk->setChecked(matchCase_);
	uiFind.backwardChk->setChecked(backward_);
	uiFind.replaceChk->setChecked(false);
	uiFind.regexpChk->setChecked(regExpMode_);
	uiFind.multiLineChk->setChecked(multiLine_);
	uiFind.wholeWordsChk->setChecked(wholeWords_);

	setReplaceMode(repl);
	if ( !multiLine_ ) {
		uiFind.findCmb->setFocus();
		uiFind.findCmb->lineEdit()->selectAll();
	}
	else {
		uiFind.mlEd->setFocus();
		uiFind.mlEd->selectAll();
	}
	uiFind.mlEd->setVisible(multiLine_);
	uiFind.findCmb->setVisible(!multiLine_);

//	resize(MainSettings::findDlgRect().size());
}

FindDlg::~FindDlg() {
	lastReplaceText_ = uiFind.replaceCmb->currentText();
	matchCase_ = uiFind.matchCaseChk->isChecked();
	backward_ = uiFind.backwardChk->isChecked();
	regExpMode_ = uiFind.regexpChk->isChecked();
	wholeWords_ = uiFind.wholeWordsChk->isChecked();
	multiLine_ = uiFind.multiLineChk->isChecked();
	if ( multiLine_ ) {
		lastString_ = uiFind.mlEd->toPlainText();
	}
	else {
		lastString_ = uiFind.findCmb->currentText();
	}

//	MainSettings::setFindDlgRect(rect());
	
	if ( strings_.contains(lastString_) )
		strings_.removeAll(lastString_);
	if ( replaces_.contains(lastReplaceText_) )
		replaces_.removeAll(lastReplaceText_);
	if ( !lastString_.isEmpty() )
		strings_.prepend(lastString_);
	if ( !lastReplaceText_.isEmpty() )
		replaces_.prepend(lastReplaceText_);
}

void FindDlg::setText(const QString& t) {
	if ( uiFind.multiLineChk->isChecked() ) {
		uiFind.mlEd->setText(t); 
		uiFind.mlEd->selectAll();
	}
	else {
		if ( !t.isEmpty() ) {
			uiFind.findCmb->insertItem(0, t);
			uiFind.findCmb->setCurrentIndex(0); 
			uiFind.findCmb->lineEdit()->selectAll();
		}
	}
}

QString FindDlg::text() const {
	if ( uiFind.multiLineChk->isChecked() )
		return uiFind.mlEd->toPlainText();
	else
		return uiFind.findCmb->currentText();
}

QString FindDlg::replaceTo() const {
	QString replStr = uiFind.replaceCmb->currentText();
	replStr.replace("\\n", "\n");
	replStr.replace("\\r", "\r");
	replStr.replace("\\t", "\t");
	return replStr;
}

Juff::SearchParams FindDlg::params() const {
	Juff::SearchParams params;
	params.findWhat = text();
	params.replaceWith = replaceTo();
	params.replace = uiFind.replaceChk->isChecked();
	params.caseSensitive = uiFind.matchCaseChk->isChecked();
	params.backwards = uiFind.backwardChk->isChecked();
	params.wholeWords = uiFind.wholeWordsChk->isChecked();
	params.regExp = uiFind.regexpChk->isChecked();
	params.multiLine = uiFind.multiLineChk->isChecked();
	return params;
}

void FindDlg::keyPressEvent(QKeyEvent* e) {
	if ( ( e->key() == Qt::Key_F || e->key() == Qt::Key_R ) && e->modifiers() & Qt::ControlModifier) {
		uiFind.replaceChk->toggle();
	}
	QDialog::keyPressEvent(e);
}

void FindDlg::setReplaceMode(bool replaceMode) {
	if (replaceMode) {
		uiFind.findBtn->setText(tr("Replace"));
//		uiFind.findBtn->setIcon(IconManager::instance()->getIcon(ID_REPLACE));
		setWindowTitle(tr("Replace"));
		uiFind.replaceCmb->setFocus();
		uiFind.replaceCmb->lineEdit()->selectAll();
	}
	else {
		uiFind.findBtn->setText(tr("Find"));
//		uiFind.findBtn->setIcon(IconManager::instance()->getIcon(ID_FIND));
		setWindowTitle(tr("Find"));
		if ( uiFind.multiLineChk->isChecked() )
			uiFind.mlEd->setFocus();
		else
			uiFind.findCmb->setFocus();
	}
	if (uiFind.replaceChk->isChecked() != replaceMode)
		uiFind.replaceChk->setChecked(replaceMode);
}

void FindDlg::multiLineChecked(bool chk) {
	if ( chk ) {
		uiFind.lowerSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
		uiFind.mlEd->setText(uiFind.findCmb->currentText());
		if ( uiFind.findCmb->hasFocus() )
			uiFind.mlEd->setFocus();
	}
	else {
		uiFind.findCmb->setEditText(uiFind.mlEd->toPlainText());
		uiFind.lowerSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
		if ( uiFind.mlEd->hasFocus() )
			uiFind.findCmb->setFocus();
	}
}

void FindDlg::regExpChecked(bool chk) {
	if ( !chk ) {
		if ( uiFind.multiLineChk->isChecked() ) {
			multiLine_ = true;
			uiFind.multiLineChk->setChecked(false);
		}
	}
	else {
		uiFind.multiLineChk->setChecked(multiLine_);
	}
}

